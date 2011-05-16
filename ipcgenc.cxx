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

static File *f_parent_begin = 0;
static File *f_parent_runtime = 0;
static File *f_parent_header = 0;
static File *f_parent_wrappers = 0;
static File *f_parent_init = 0;
static File *f_parent_table = 0;
static String *module = 0;

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

    SWIG_library_directory("ipcgen");
    Preprocessor_define((DOH *) "SWIG_CPLUSPLUS_CAST", 0);
    Preprocessor_define("SWIGIPCGEN_PARENT 1", 0);
    SWIG_config_file("ipcgenc.swg");
    SWIG_typemap_lang("ipcgen");
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
    String *outfile_h = Getattr(n, "outfile_h");

    if (!outfile) {
      Printf(stderr, "Unable to determine outfile\n");
      SWIG_exit(EXIT_FAILURE);
    }

    f_parent_begin = NewFile(outfile, "w", SWIG_output_files());
    if (!f_parent_begin) {
      FileErrorDisplay(outfile);
      SWIG_exit(EXIT_FAILURE);
    }

    f_parent_runtime = NewString("");
    f_parent_init = NewString("");
    f_parent_header = NewString("");
    f_parent_wrappers = NewString("");

    /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("header", f_parent_header);
    Swig_register_filebyname("wrapper", f_parent_wrappers);
    Swig_register_filebyname("begin", f_parent_begin);
    Swig_register_filebyname("runtime", f_parent_runtime);
    Swig_register_filebyname("init", f_parent_init);

    Swig_banner(f_parent_begin);
    Swig_banner(f_parent_wrappers);

    Printf(f_parent_runtime, "\n");
    Printf(f_parent_runtime, "#define SWIGIPCGEN\n");

    /* Set module name */
    module = Copy(Getattr(n, "name"));

    Printf(f_parent_wrappers, "\n");
    Printf(f_parent_wrappers, "#ifndef SWIG_%s_WRAP_H_\n", module);
    Printf(f_parent_wrappers, "#define SWIG_%s_WRAP_H_\n\n", module);

    // This sets the template for how wrapper names will be generated. Notice we
    //  are actually writing implementations of the methods/functions.
    Swig_name_register("wrapper", "wrap_child_%f");

    /* emit code */
    Language::top(n);

    /* Close all of the files */
    Dump(f_parent_runtime, f_parent_begin);
    Dump(f_parent_header, f_parent_begin);

    Dump(f_parent_wrappers, f_parent_begin);

    Wrapper_pretty_print(f_parent_init, f_parent_begin);

    Delete(f_parent_header);
    Delete(f_parent_wrappers);
    Delete(f_parent_init);

    Close(f_parent_begin);
    Delete(f_parent_runtime);
    Delete(f_parent_begin);

    return SWIG_OK;
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

    Printv(f->def, "SWIGINTERN ", SwigType_str(returnType,NIL), " ", wname, "(MessageChannel& calling, MessageChannel& returning",NIL);

    // Now walk the function parameter list and generate code to get arguments

    /* Attach the standard typemaps */
    Swig_typemap_attach_parms("ctype", plist, f);
    Swig_typemap_attach_parms("imtype", plist, f);
    emit_attach_parmmaps(plist, f);

    Parm *p;

    // declare the variable 'result' to hold the return value
    SwigType* rettype = Getattr(n, "type");
    Wrapper_add_localv(f, "result", rettype, "result", NIL);

    for (p = plist; p; p = nextSibling(p)) 
    {
      String *im_param_type = NewString("");

      String *arg = NewString("");
      String* lname = Getattr(p, "lname");
      SwigType* ptype = Getattr(p, "type");

      // make sure there is a typemap for this type. Everything needs a typemap
      if (!Getattr(p,"tmap:in"))
      {
        Swig_error(input_file,line_number,"Typemaps must be defined for all parameters. Function \"%s\" is missing type \"%s\"\n",symname,SwigType_str(ptype, 0));
        return SWIG_ERROR;
      }

      // make sure there is a typemap for this type. Everything needs a typemap
      if (!Getattr(p,"tmap:imtype"))
      {
        Swig_error(input_file,line_number,"Intermediate typemaps must be defined for all parameters. Function \"%s\" is missing type \"%s\"\n",symname,SwigType_str(ptype, 0));
        return SWIG_ERROR;
      }

      Printf(arg, "p%s", lname);

      Printv(f->def, "," , SwigType_str(ptype,NIL), " ", arg, NIL);

      // Get typemap for this argument
      tm = Getattr(p, "tmap:in");
      Replaceall(tm, "$input", arg);
      Setattr(p, "emit:input", arg);
      Printf(f->code, "%s\n", tm);

      tm = Getattr(p, "tmap:imtype");
      Replaceall(tm, "$input", arg);
      Setattr(p, "emit:input", arg);
      Printf(f->code, "%s\n", tm);

      Delete(arg);
      Delete(im_param_type);
    }

    Printv(f->def, ")\n{", NIL);

    // Emit all of the local variables for holding arguments.
    emit_parameter_variables(plist, f);
    emit_return_variable(n,returnType,f);

    //emit_args(t,l,f);

    // need to save the swig state and change the wrap function

    //emit_action(n);
    // to emit action code the wrapper function name and wrapper params needs
    //  to be set.
    Setattr(n, "wrap:parms", plist);
    Setattr(n, "wrap:name", wname);
    String* actioncode = NewString("");
    Printv(actioncode,"message.flip();\n", "calling.send(message);\n", "message.clear();\n","message.flip();\n",NIL);

    tm = Swig_typemap_lookup_out("out", n, "result", f, actioncode);
    if (tm)
    {
      Replaceall(tm, "$result", "result");
      Printf(f->code, "%s\n", tm);
    }
    else
      Printv(f->code,actioncode,NIL);

    bool is_void_return = (Cmp(returnType, "void") == 0);
    if (!is_void_return)
      Printv(f->code, "    return result;\n", NIL);

    // need to restore the swig state
    Printv(f->code,"}",NIL);

    Wrapper_print(f,f_parent_wrappers);
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
