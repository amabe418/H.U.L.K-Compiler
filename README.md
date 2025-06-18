# H.U.L.K-Compiler

HULK (Havana University Language for Kompilers) is a didactic, type-safe, object-oriented and incremental programming language, designed for the course Introduction to Compilers in the Computer Science major at University of Havana.


no no, a ver, vayamos mejor por partes, cambiemos primero el function declaration, como puedes ver,  al inicio, en la tabla de simbolos se guardan los nombres y los symbol kind, sin embargo, el type no se llena en la tabla de simbolos, porque su trabajo es llenarse en el type checker, asi que haremos lo siguiente, en typechecker, arreglaremos primero las funciones, y lo que haremos es, buscar la funcion en la tabla de simbolos y meternos en ese scope, luego miramos los parametros y buscamos el tipo de esos parametros teniendo en cuenta para que se usan, es decir, si se tiene la funcion f(a,b) => a+b, registramos en el symbol table