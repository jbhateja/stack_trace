
#include <iostream>
#include <stdio.h>
#include <ucontext.h>
#include <bits/wordsize.h>

// 

#if __WORDSIZE == 64
   typedef uint64_t regType;
   #define FRAME_REG  REG_RBP
   #define IP_REG     REG_RIP
#else
   typedef uint32_t regType;
   #define FRAME_REG  REG_EBP
   #define IP_REG     REG_EIP
#endif

class PrintTrace {
   public:
       PrintTrace(ucontext_t * u)  : _uc(u) {} 
       void dumpTrace(regType *fp,regType  *ip,int frameNumber);
       void analyzeContext();

   private:
       PrintTrace(const PrintTrace &) = delete;
       PrintTrace & operator = (const PrintTrace &) = delete;

       ucontext_t * _uc;
};

void PrintTrace::analyzeContext() {
     int frame = 0;
     mcontext_t mc = _uc->uc_mcontext;	
     regType* cfp = reinterpret_cast<regType*>(mc.gregs[FRAME_REG]);      
     regType* cip = reinterpret_cast<regType*>(mc.gregs[IP_REG]);      
     dumpTrace(cfp,cip,frame);
}

void PrintTrace::dumpTrace(regType *fp,regType* ip,int frame) {
     if(!fp) {
        return;
     }
     fprintf(stderr,"[%d] Current IP : %p , Current FP : %p .. \n",
           frame,ip,fp);
     dumpTrace(reinterpret_cast<regType*>(fp[0]),
          reinterpret_cast<regType*>((fp+1)[0]),frame+1);
}


#define TRACE            \
   do {                  \
      ucontext_t uc;     \
      getcontext(&uc);   \
      PrintTrace t(&uc); \
      t.analyzeContext();\
   } while (0)    


int func3() {
   int i = 10;
   int j = 20;
   TRACE; 
   return i+j;
}

int func2() {
   int i = 10;
   return i+func3();
}

int func1() {
   int i = 10;
   return i+func2();
}

int main() {
   return func1();
}
