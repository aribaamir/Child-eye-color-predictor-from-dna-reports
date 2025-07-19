=== Compile Instructions ===

To build this program on Windows with MinGW:

1. Make sure SFML is installed and its include and lib folders are accessible.

2. Open Command Prompt, navigate to the project folder, and run:

   g++ -o EyeColorPredictor EyeColorPredictor.cpp -lsfml-graphics -lsfml-window -lsfml-system

3. Run the executable:

   EyeColorPredictor.exe

Note:
- The program uses Windows-specific file dialogs (`windows.h`, `commdlg.h`), so it must be compiled on Windows.
- You need eye.png and arial.ttf in the same directory as the executable.
