#include <Python.h>

int main(int argc, char *argv[]) {
  Py_SetProgramName(argv[0]);  /* optional but recommended */
  Py_Initialize();
  
  char const * this_script =
    "from sys import path\n"
    "path.insert(0, '" PATH "')\n";
    
  PyRun_SimpleString(this_script);
  Py_Main(argc, argv);
  Py_Finalize();
  return 0;
}
