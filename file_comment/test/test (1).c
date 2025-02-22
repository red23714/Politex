int myproc1()
{
   const char* nc = " Don't touch text b/* inside */ quoted strings ";

   // This is comment
   printf(" This is // not comment ");
   
   printf(" This is /* not comment */ too");

   myproc2();  // This is comment

   /* This is comment too */ myproc3();

   myproc4();

   /* This is
      multiline comment */

   myproc5();

   // This is "cut-line"  \
and prolongation of single-line comment

   myproc6();

   printf(  " This is multiline \
string constant                 \
without /* comment. Don't /*  \
touch me. */ please" );

   /* This is multiline comment
      // this is single-line comment inside multiline */

   myproc7();

   /* This is multiline comment ...
       /* prolongation ....
       end of comment here -> */  

   myproc8();

   return 1;
}
