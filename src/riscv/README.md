# RISCV 組合語言產生


參考: [RISC-V assembly (31 Dec 2016)](https://www.imperialviolet.org/2016/12/31/riscv.html)

```
root@localhost:~/ccc/c2riscv/test# riscv64gcc sum.v.s -o sum.v.o
root@localhost:~/ccc/c2riscv/test# spike -m128 pk sum.v.o
i=1 s=0 n=10
i=1 s=1
i=2 s=3
i=3 s=6
i=4 s=10
i=5 s=15
i=6 s=21
i=7 s=28
i=8 s=36
i=9 s=45
i=10 s=55
sum(10)=55
```

# LD/LW/LWU


* https://tclin914.github.io/categories/RISC-V/

* [RISC-V 指令集架構介紹 - RV64I](https://tclin914.github.io/464e7cd9/)

ld/lw/lwu rd, rs1, simm12
常數部分為 sign-extended 12-bit，載入位址則為 rs1暫存器加上 sign-extended 12-bit，LD為載入 64-bit資料寫入 rd暫存器，LW/LWU為載入 32-bit資料分別做 unsigned/signed extension成 64-bit後寫入 rd暫存器。

