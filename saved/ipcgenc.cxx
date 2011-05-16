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

    SWIG_library_directory("ipcgen");
    Preprocessor_define((DOH *) "SWIG_CPLUSPLUS_CAST", 0);
    Preprocessor_define("SWIGIPCGEN_PARENT 1", 0);
    SWIG_config_file("ipcgenp.swg");
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
      Printv(f_parent_wrappers,"\n",_S,"}\n\n", NIL);
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

        Printv(f_parent_wrappers,_S,"namespace ",Getattr(curns,"name"), "\n", _S, "{\n", NIL);
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
    String* parmt = NewString("");
    String* pstructDef = NewString("");
    String* fdef = NewString("");

    Printv(parmt, wname, "_pt", NIL);

    Printv(pstructDef, "struct " , parmt, "{\n",NIL);

    Printv(fdef, "SWIGINTERN ", SwigType_str(returnType,NIL), " ", wname, "(const struct ", parmt, "& args)",NIL);
    Printv(fdef, "\n{", NIL);

    // Now walk the function parameter list and generate code to get arguments
//    printf("Num Args:%d,%d\n",(int)emit_num_arguments(plist),emit_num_required(plist));

    /* Attach the standard typemaps */
    emit_attach_parmmaps(plist, f);

    Parm *p;
//    bool alreadyDidAParam = false;
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

      Printf(arg, "args.%s", lname);

      Printv(pstructDef, SwigType_str(ptype,NIL), " ", lname, ";\n", NIL);
//      alreadyDidAParam = true;

      // Get typemap for this argument
      tm = Getattr(p, "tmap:in");
      Replaceall(tm, "$input", arg);
      Setattr(p, "emit:input", arg);
      Printf(f->code, "%s\n", tm);

      Delete(arg);
    }

    Printv(pstructDef, "};\n",NIL);

    Printv(f->def, pstructDef, NIL);
    Printv(f->def, fdef, NIL);

    // Emit all of the local variables for holding arguments.
    emit_parameter_variables(plist, f);

    //emit_args(t,l,f);

    // need to save the swig state and change the wrap function

    //emit_action(n);
    // to emit action code the wrapper function name and wrapper params needs
    //  to be set.
    Setattr(n, "wrap:parms", plist);
    Setattr(n, "wrap:name", wname);
    String* actioncode = emit_action(n);
    Printv(f->code,actioncode,NIL);
    // need to restore the swig state

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
