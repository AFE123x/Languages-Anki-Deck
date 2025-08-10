# 5. Debugging

## What is a device probe?

- A device probe is a device that lets a computer communicate and program non-volatile flash of the MCU.

## Using SEGGER probes

- The SEGGER offers a lineup of different probes
    - for example, SEGGER J-link and SEGGER J-Trace
- SEGGER offers software that integrates with their tools. 

## Segger J-Trace

- This is a debugger with full trace support.
- it records all instructions, which means you can send them out via pins which can let you go line by line. 

## embedded systems - common file types

- There are multiple file types here:
    - ELF Files (Executable and Linkable Format): store the executable binary, but can contain metadata, like links to original source code, useful for debugging.
    - `*.bin` - straight binary file (just 1s and 0s), which acn be directly flashed onto the Microcontroller.
    - `*.hex` - similar to binary , but contains absolute memory address, and contents at memory address.
    - `*.svd` - contains information mapping registers and descriptions to the memory map of target device. 

## Test driven developement

- **Test-Driven Development (TDD)** is a development methodology.
- Instead of writing code with the intent to debug, write tests, then develop the production code until it passes the tests.

## Static Analysis

- static analysis is the analysis of code without being executed. 
- It checks for syntax related errors.