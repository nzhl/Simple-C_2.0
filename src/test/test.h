
void fail_with_msg(char *filename, int line, char *msg);
void expect_int(char *filename, int line, int expect, int actual);
void expect_ptr(char *filename, int line, void *expect, void *actual);


#define fail(msg) fail_with_msg(__FILE__, __LINE__, msg)
#define expecti(expect, actual) expect_int(__FILE__, __LINE__, expect, actual)
#define expectp(expect, actual) expect_ptr(__FILE__, __LINE__, expect, actual)


