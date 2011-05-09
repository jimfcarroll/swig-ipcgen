/* -----------------------------------------------------------------------------
 * This file is part of SWIG, which is licensed as a whole under version 3 
 * (or any later version) of the GNU General Public License. Some additional
 * terms also apply to certain portions of SWIG. The full details of the SWIG
 * license and copyrights can be found in the LICENSE and COPYRIGHT files
 * included with the SWIG source code as distributed by the SWIG developers
 * and at http://www.swig.org/legal.html.
 *
 * python.cxx
 *
 * Python language module for SWIG.
 * ----------------------------------------------------------------------------- */

#include "trace.h"

#include "swigmod.h"
#include "cparse.h"

#include <stdio.h>

static int no_header_file = 0;
static File *f_begin = 0;
static File *f_runtime = 0;
static File *f_runtime_h = 0;
static File *f_header = 0;
static File *f_wrappers = 0;
static File *f_directors = 0;
static File *f_directors_h = 0;
static File *f_init = 0;
static int nortti = 0;
static String *module = 0;
static int curdepth = 0;

#define INDENT "  "
#define MAX_INDENT 20

static String** makespace()
{
  TRACE;

  String** ret = new String*[MAX_INDENT];
  ret[0] = NewString("");
  for (int i = 1; i < MAX_INDENT; i++)
  {
    ret[i] = NewString("");
    Printf(ret[i],"%s%s",ret[i-1],INDENT);
  }

  return ret;
}

static String** spaces = makespace();

#define _S spaces[curdepth]

static String* currentClassName = NULL;
static bool hasConstructor = false;
static String* currentNamespace = NewString("");

class IPCGEN:public Language {

public:
  IPCGEN()
  { 
    TRACE;
    director_multiple_inheritance = 1;
    director_language = 1;
  }

  /* ------------------------------------------------------------
   * main()
   * ------------------------------------------------------------ */

  virtual void main(int argc, char *argv[])
  {
    TRACE;

    for (int i = 1; i < argc; i++) {
      if (argv[i]) {
        if (strcmp(argv[i], "-noh") == 0) {
	  no_header_file = 1;
	  Swig_mark_arg(i);
        }
      }
    }

    Preprocessor_define((DOH *) "SWIG_CPLUSPLUS_CAST", 0);
    Preprocessor_define("SWIGIPCGEN 1", 0);
    SWIG_typemap_lang("ipcgen");
    SWIG_config_file("ipcgen.swg");
  }


  /* ------------------------------------------------------------
   * top()
   * ------------------------------------------------------------ */

  virtual int top(Node *n)
  {
    TRACE;
    /* check if directors are enabled for this module.  note: this 
     * is a "master" switch, without which no director code will be
     * emitted.  %feature("director") statements are also required
     * to enable directors for individual classes or methods.
     *
     * use %module(directors="1") modulename at the start of the 
     * interface file to enable director generation.
     */
    String *mod_docstring = NULL;
    {
      Node *mod = Getattr(n, "module");
      if (mod) {
	Node *options = Getattr(mod, "options");
	if (options) {
	  int dirprot = 0;
	  if (Getattr(options, "dirprot")) {
	    dirprot = 1;
	  }
	  if (Getattr(options, "nodirprot")) {
	    dirprot = 0;
	  }
	  if (Getattr(options, "directors")) {
	    allow_directors();
	    if (dirprot)
	      allow_dirprot();
	  }
	}
      }
    }

    /* Initialize all of the output files */
    String *outfile = Getattr(n, "outfile");
    String *outfile_h = !no_header_file ? Getattr(n, "outfile_h") : 0;

    if (!outfile) {
      Printf(stderr, "Unable to determine outfile\n");
      SWIG_exit(EXIT_FAILURE);
    }

    f_begin = NewFile(outfile, "w", SWIG_output_files());
    if (!f_begin) {
      FileErrorDisplay(outfile);
      SWIG_exit(EXIT_FAILURE);
    }

    f_runtime = NewString("");
    f_init = NewString("");
    f_header = NewString("");
    f_wrappers = NewString("");
    f_directors_h = NewString("");
    f_directors = NewString("");

    if (directorsEnabled()) {
      if (!no_header_file) {
	f_runtime_h = NewFile(outfile_h, "w", SWIG_output_files());
	if (!f_runtime_h) {
	  FileErrorDisplay(outfile_h);
	  SWIG_exit(EXIT_FAILURE);
	}
      } else {
	f_runtime_h = f_runtime;
      }
    }

    /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("header", f_header);
    Swig_register_filebyname("wrapper", f_wrappers);
    Swig_register_filebyname("begin", f_begin);
    Swig_register_filebyname("runtime", f_runtime);
    Swig_register_filebyname("init", f_init);
    Swig_register_filebyname("director", f_directors);
    Swig_register_filebyname("director_h", f_directors_h);

    Swig_banner(f_begin);
    Swig_banner(f_runtime_h);

    Printf(f_runtime, "\n");
    Printf(f_runtime, "#define SWIGIPCGEN\n");

    if (directorsEnabled()) {
      Printf(f_runtime, "#define SWIG_DIRECTORS\n");
    }

    /* Set module name */
    module = Copy(Getattr(n, "name"));

    Printf(f_runtime_h, "\n");
    Printf(f_runtime_h, "#ifndef SWIG_%s_WRAP_H_\n", module);
    Printf(f_runtime_h, "#define SWIG_%s_WRAP_H_\n\n", module);
    if (directorsEnabled())
    {
      Printf(f_directors, "\n\n");
      Printf(f_directors, "/* ---------------------------------------------------\n");
      Printf(f_directors, " * C++ director class methods\n");
      Printf(f_directors, " * --------------------------------------------------- */\n\n");
      if (outfile_h)
        Printf(f_directors, "#include \"%s\"\n\n", Swig_file_filename(outfile_h));
    }

    // This sets the template for how wrapper names will be generated. Notice we
    //  are actually writing implementations of the methods/functions.
    Swig_name_register("wrapper", "%f");

    /* emit code */
    Language::top(n);

    if (directorsEnabled()) {
      // Insert director runtime into the f_runtime file (make it occur before %header section)
      Swig_insert_file("director.swg", f_runtime);
    }

    /* Close all of the files */
    Dump(f_runtime, f_begin);
    Dump(f_header, f_begin);

    if (directorsEnabled()) {
      Dump(f_directors_h, f_runtime_h);
      Printf(f_runtime_h, "\n");
      Printf(f_runtime_h, "#endif\n");
      if (f_runtime_h != f_begin)
	Close(f_runtime_h);
      Dump(f_directors, f_begin);
    }

    Dump(f_wrappers, f_begin);
    Wrapper_pretty_print(f_init, f_begin);

    Delete(f_header);
    Delete(f_wrappers);
    Delete(f_init);
    Delete(f_directors);
    Delete(f_directors_h);

    Close(f_begin);
    Delete(f_runtime);
    Delete(f_begin);

    return SWIG_OK;
  }

  /**
   *
   */
  virtual int namespaceDeclaration(Node *n)
  {
    TRACE;

    int ret = Language::namespaceDeclaration(n);

    if (Getattr(n, "ipcgen:opened"))
    {
      curdepth--;
      Printv(f_runtime_h,"\n",_S,"}\n\n", NIL);
    }

    return ret;
  }

  void openNamespace(Node* n)
  {
    TRACE;
    bool foundNamespaceNode = false;
    Node *curns = n;

    String* namespaceConst = NewString("namespace");

    while (!foundNamespaceNode && curns)
    {
      if (Strcmp(namespaceConst,nodeType(curns)))
        curns = parentNode(curns);
      else
        foundNamespaceNode = true;
    }

    if (foundNamespaceNode)
    {
      if (!Getattr(curns, "ipcgen:opened"))
      {
        Node* parentns = parentNode(curns);
        if (parentns)
          openNamespace(parentns);

        Printv(f_runtime_h,_S,"namespace ",Getattr(curns,"name"), "\n", _S, "{\n", NIL);
        curdepth++;

        String* nextns = NewString("");
        Printv(nextns,currentNamespace,"::",Getattr(curns,"name"),NIL);
        Delete(currentNamespace);
        currentNamespace = nextns;
        Setattr(curns, "ipcgen:opened", "true");
      }
    }

    Delete(namespaceConst);
  }

  /* ----------------------------------------------------------------------
   * functionWrapper()
   * ---------------------------------------------------------------------- */

  virtual int functionWrapper(Node *n) 
  {
    TRACE;

    String *symname = Getattr(n, "sym:name");
    SwigType *returnType = Getattr(n, "type");
    ParmList *plist = Getattr(n, "parms");
    String *tm; // various typemaps

    // A new wrapper function object
    Wrapper *f = NewWrapper();

    // Make a wrapper name for this function
    String *wname = Swig_name_wrapper(symname);

    Printv(f->def, SwigType_str(returnType,NIL), " ", wname, "(", NIL);

    // Emit all of the local variables for holding arguments.
    emit_parameter_variables(plist, f);

    /* Attach the standard typemaps */
    emit_attach_parmmaps(plist, f);

    //emit_args(t,l,f);

    //emit_action(n);

    // Now walk the function parameter list and generate code to get arguments
    printf("Num Args:%d,%d\n",(int)emit_num_arguments(plist),emit_num_required(plist));

    Parm *p;
    bool alreadyDidAParam = false;
    for (p = plist; p; p = nextSibling(p)) 
    {
      // make sure there is a typemap for this type. Everything needs a typemap
      if (!Getattr(p,"tmap:in"))
      {
        Swig_error(input_file,line_number,"Typemaps must be defined for all parameters.\n");
        return SWIG_ERROR;
      }

      String *arg = NewString("");
      String* lname = Getattr(p, "lname");
      SwigType* ptype = Getattr(p, "type");

      Printf(arg, "p_%s", lname);

      Printv(f->def, (alreadyDidAParam ? "," : "") , SwigType_str(ptype,NIL), " ", arg, NIL);

      alreadyDidAParam = true;

      // Get typemap for this argument
      tm = Getattr(p, "tmap:in");
      Replaceall(tm, "$input", arg);
      Setattr(p, "emit:input", arg);
      Printf(f->code, "%s\n", tm);

      Delete(arg);
    }

    Printv(f->def, ")\n{", NIL);

    // need to save the swig state and change the wrap function

    // to emit action code the wrapper function name and wrapper params needs
    //  to be set.
    Setattr(n, "wrap:parms", plist);
    Setattr(n, "wrap:name", wname);
    String* actioncode = emit_action(n);
    Printv(f->code,actioncode,NIL);
    // need to restore the swig state

    Wrapper_print(f,f_runtime_h);
  }

};


/* -----------------------------------------------------------------------------
 * swig_python()    - Instantiate module
 * ----------------------------------------------------------------------------- */

static Language *new_swig_ipcgen() {
  TRACE;
  return new IPCGEN();
}

extern "C" Language *swig_ipcgen(void) {
  TRACE;
  return new_swig_ipcgen();
}
