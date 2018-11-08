#pragma once
#ifndef MINUTILS_MINHELPERS_H_INCLUDED
#define MINUTILS_MINHELPERS_H_INCLUDED

/// Macro for disallowing copy constructor and assignment operator for
/// @ClassName.
#define DISALLOW_COPY_AND_ASSIGN(ClassName)     \
  private:\
  ClassName(const ClassName &); \
  void operator=(const ClassName &);

/// Macro for easy defining a member with its getter and setter methods.o
#define DEFINE_GET_SET_MEMBER(Type,name)                   \
  public:                                                  \
  const Type & name() const { return name##_; }            \
   void set_##name(const Type &value) { name##_ = value; } \
 private:                                                  \
   Type name##_;

/// Some times it is necessary to have a variable, which will not be used in
/// some the context. For example if the code which uses the variable is under
/// conditional compilation. For such cases it is convenient to use this macro
/// to suppress corresponding warnings.
///
/// Remark: use in case of strong need only. Remove unused variables if they
/// are not needed at all.
#define SUPPRESS_UNUSED_VARIABLE(x) (void)(x);


/// Macro to mark a variable as unused right inside the function signature.
#define UNUSED_VAR(x)

#endif // #ifndef MINUTILS_MINHELPERS_H_INCLUDED
