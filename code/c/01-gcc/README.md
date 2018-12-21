# GCC

## 輸出 a.out

```
$ gcc sum.c
```

## 使用 -o 參數

```
$ gcc sum.c -o sum
```

## 產生組合語言

```
$ gcc -S sum.c -o sum.s
```

## 產生有詳細註解的組合語言

```
$ gcc -S -fverbose-asm sum.c -o sum.s
```

## 產生 RISCV 有詳細註解的組合語言

```
$ riscv64-unknown-elf-gcc -S -fverbose-asm sum.c -o sum.s
```
