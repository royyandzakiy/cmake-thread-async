# C++ Threading Cheatsheet

Quick reference for threading and async patterns in C++

## Setup
1. **Compiler**: MSVC (v143) or any C++17+ compatible compiler
2. **IDE**: VS Code with CMake extension
3. **Build**: CMake (no external dependencies needed)

## Cases Overview

### 1️⃣ Basic Timing
- `this_thread::sleep_for`
- Measures sleep duration accurately

### 2️⃣ Synchronization Basics
- `mutex` + `lock_guard`
- `atomic` counter
- Regular (unsafe) counter

### 3️⃣ Advanced Synchronization
- `unique_lock` with condition variable
- `try_lock` attempts
- Thread signaling pattern

### 4️⃣ Non-blocking Locks
- `try_lock` with retry logic
- Critical vs non-critical work sections
- Lock acquisition attempts

### 5️⃣ Async Patterns
- `promise`/`future` for thread communication
- `async` with launch policies
- Multiple parallel tasks

### 6️⃣ Detach vs Async
- `thread.detach()` limitations
- `async` advantages (return values, completion tracking)

## Usage
1. Open in VS Code
2. Build with CMake
3. Run executable - all cases execute sequentially

## Key Shortcuts
- Case 1: Basic timing
- Case 2: Shared data protection
- Case 3: Thread coordination
- Case 4: Non-blocking approaches
- Case 5: Async operations
- Case 6: Fire-and-forget vs managed

> **Note**: This is meant as a quick reference - the code is self-documenting with clear output markers.