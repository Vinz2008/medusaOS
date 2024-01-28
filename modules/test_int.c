int main() {
  char* test = "Hello from module\n";
  int ret;
  asm volatile("int $0x80"
               : "=a"(ret)
               : "0"(1), // SYS_WRITE
                 "b"(1) /* fd */, "c"(test), "d"(19) /* size */);
}
