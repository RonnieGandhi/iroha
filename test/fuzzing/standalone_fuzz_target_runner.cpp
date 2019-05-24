// Copyright 2017 Google Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");

// Example of a standalone runner for "fuzz targets".
// It reads all files passed as parameters and feeds their contents
// one by one into the fuzz target (LLVMFuzzerTestOneInput).
// This runner does not do any fuzzing, but allows us to run the fuzz target
// on the test corpus (e.g. "do_stuff_test_data") or on a single file,
// e.g. the one that comes from a bug report.

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

// Define LLVMFuzzerMutate to avoid link failures for targets that use it
// with libFuzzer's LLVMFuzzerCustomMutator.
extern "C" size_t LLVMFuzzerMutate(uint8_t *Data, size_t Size, size_t MaxSize) {
  assert(false && "LLVMFuzzerMutate should not be called from afl_driver");
  return 0;
}

// Forward declare the "fuzz target" interface.
// We deliberately keep this inteface simple and header-free.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    std::ifstream in(argv[i]);
    in.seekg(0, in.end);
    size_t length = in.tellg();
    in.seekg(0, in.beg);
    std::cout << "Reading " << length << " bytes from " << argv[i] << std::endl;
    // Allocate exactly length bytes so that we reliably catch buffer overflows.
    std::vector<char> bytes(length);
    in.read(bytes.data(), bytes.size());
    assert(in);
    LLVMFuzzerTestOneInput(reinterpret_cast<const uint8_t *>(bytes.data()),
                           bytes.size());
    std::cout << "Execution successful" << std::endl;
  }
}
