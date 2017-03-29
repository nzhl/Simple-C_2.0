## Simple-C_2.0

Finally i go back to write compiler again. But I will not base on the [Simple C](https://github.com/zhang77595103/Simple-C) one since it's writen in Java, instead this time I will write an C++ version of compiler, and this version will be much more close to the real c language. 

### The grammar

Currently it's actually not that like a powerful language, it just a minor C which is a subset of c. There are several difference comparing with modern C. 

1. You can only define all your variable at the front of the function body.
2. It do not support global variable.
3. Support if/else, while, return statement
4. Only int and char is supported, actually it can have pointer inside as long as you do not use it.
5. Do not support function call.

Here is an example that basically cover all the language feature now.


```c

int main(int argc, char **argv){
    int x, y, z;

    z = 0;

    x = y = 10;
    while(x){
        y = y + 5;
        x = x - 1;
    }
    return 6;

    if(z){
        y = y + y;
    }else{
        y = 40;
    }

    return y;
}

```

This is what it will generate :

```asm
.global _main
_main:
pushl %ebp
movl %esp, %ebp
subl $12, %esp
pushl $0
    popl %eax
    movl %eax, -12(%ebp)
pushl -12(%ebp)
    pushl $10
    popl %eax
    movl %eax, -8(%ebp)
pushl -8(%ebp)
    popl %eax
    movl %eax, -4(%ebp)
pushl -4(%ebp)
    while_l0:
    movl -4(%ebp), %eax
    pushl %eax
    popl %eax
    testl %eax, %eax
    je while_l1
    pushl $5
    movl -8(%ebp), %eax
    addl (%esp), %eax
    pushl %eax
    popl %eax
    movl %eax, -8(%ebp)
pushl -8(%ebp)
    pushl $1
    movl -4(%ebp), %eax
    subl (%esp), %eax
    pushl %eax
    popl %eax
    movl %eax, -4(%ebp)
pushl -4(%ebp)
    jmp while_l0
    while_l1:
    pushl $6
    popl %eax
    leave
    ret
```

Actually it's x86 assembly that **can be actually accepted by gcc**, but i only test it on my mac, i am not sure if other linux version can run it. 



### Have A Taste 

1. install cmake and make.
2. cmake CMakeLists.txt && make
3. ./Simple-C_2.0 > test.s
4. gcc -m32 test.s && ./a.out && echo $? (if nothing got wrong, you should got the return value in you screen )

### Function Implemented  

---------------28 /  3 / 2017-------------------

- [x] lexer.

---------------29 /  3 / 2017-------------------


- [x]  parser. (just poor implementation, part of it)


### TODO :

- [ ] support function call
- [ ] support pointer
