#!/usr/bin/env python3

import difflib
import os
import re
import subprocess
import unittest
import warnings

picoquant = "./src/picoquant"
binary_file_pattern = re.compile("v.+\.([hpt]hd|t3r|[ph]t[23]|ptu)$")


def binary_file_paths(sample_data_dir="sample_data"):
    for root, dirs, basenames in os.walk("sample_data"):
        for basename in filter(binary_file_pattern.match, basenames):
            path = os.path.join(root, basename)
            yield path


def run(file_in, *args):
    cmd = [picoquant, "--file-in", file_in, *args]
    with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE) as p:
        stdout, stderr = p.communicate()
        raw = stdout.decode()
        err = stderr.decode()
    return raw


warnings.warn(
    """The current decoded test data were generated with an unknown version of picoquant. 
If you observe a failure for one of these data files, check whether your current version 
of picoquant appear to be generating reasonable data -- it might be the case that there was
a bug in an old version.
If you run into an issue, contact the maintainer, who can work with you to verify whether 
the output is correct.

This warning will be removed once all existing data has a verified decoded value"""
)


class PicoquantTestCase(unittest.TestCase):
    def test_data(self):
        for binary_file_path in binary_file_paths():
            with self.subTest(binary_file_path=binary_file_path):
                data_path = binary_file_path + ".data"
                if not os.path.exists(data_path):
                    self.skipTest(
                        r"decoded .data file not available for {binary_file_path=}"
                    )

                content = run(binary_file_path).strip().split("\n")
                with open(data_path) as f:
                    reference = f.read().strip().split("\n")

                content = content[: len(reference)]
                self.assertTrue(content == reference)

    def test_header(self):
        for binary_file_path in binary_file_paths():
            with self.subTest(binary_file_path=binary_file_path):
                header_path = binary_file_path + ".header"
                if not os.path.exists(header_path):
                    self.skipTest(
                        f"decoded .header file not available for {binary_file_path=}"
                    )

                content = run(binary_file_path, "--header-only")
                with open(header_path, "rb") as f:
                    reference = f.read().decode()

                self.assertTrue(content == reference)

    def test_resolution(self):
        for binary_file_path in binary_file_paths():
            with self.subTest(binary_file_path=binary_file_path):
                resolution_path = binary_file_path + ".resolution"
                if not os.path.exists(resolution_path):
                    self.skipTest(
                        "decoded .resolution file not available for {binary_file_path=}"
                    )

                content = run(binary_file_path, "--resolution-only")
                with open(resolution_path, "rb") as f:
                    reference = f.read().decode()

                self.assertTrue(content == reference)


if __name__ == "__main__":
    unittest.main()
