#!/usr/bin/env python3

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

class PicoquantDataTestCase(unittest.TestCase):
    def test_data(self):
        warnings.warn(
"""The current decoded test data were generated with an unknown version of picoquant. 
If you observe a failure for one of these data files, check whether your current version 
of picoquant appear to be generating reasonable data -- it might be the case that there was
a bug in an old version.
If you run into an issue, contact the maintainer, who can work with you to verify whether 
the output is correct.

This warning will be removed once all existing data has a verified decoded value""")
        for binary_file_path in binary_file_paths():
            with self.subTest(binary_file_path=binary_file_path):
                data_path = binary_file_path + ".data"
                if not os.path.exists(data_path):
                    continue

                content = run(binary_file_path).strip().split("\n")
                with open(binary_file_path + ".data") as f:
                    reference = f.read().strip().split("\n")

                content = content[:len(reference)]
                self.assertTrue(content == reference)

if __name__ == "__main__":
    unittest.main()
