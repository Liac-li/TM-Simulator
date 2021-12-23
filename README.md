# 多带图灵机

#### 运行方法

- 编译得到可执行文件：
  直接在根目录下运行 Makefile
  ```
    make
  ```
  会在根目录中生成一个命名为 `turing` 的可执行文件
- 运行图灵机模拟器：
  可执行文件交互为：
  ```
    $ ./turing [-v|--verbose] [-h|--help] <tm> <input>
  ```
  
  比如可以:
  ```
    $ ./turing -v ./palindrome_detector_2tapes.tm 10101 
  ```