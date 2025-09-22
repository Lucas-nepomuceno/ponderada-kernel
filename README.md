# MORSE em 1932: Aprendendo MORSE com o Gov

&emsp; Este repositório contém o bootloader `kernel.asm` que chama uma função externa `kernel.c` - via o arquivo `link.ld` - que contém o jogo autoral interativo MORSE em 1932: Aprendendo MORSE com o GOV. O primeiro passo foi criar o bootloader com base no seguinte artigo [Kernels 201 - Let’s write a Kernel with keyboard and screen support](https://arjunsreedharan.org/post/99370248137/kernels-201-lets-write-a-kernel-with-keyboard). Esse tutorial ensina a criar um "kernel x86 que boota usando GRUB, roda em modo protegido e mostra uma _string_ na tela" (Sreedharan, 2014). A partir dele o restante da aplicação foi construído. Esse restante pode ser separado em: arquivos auxiliares, introducao e funções principais.  

&emsp; Em primeiro lugar, foram criados os arquivos auxiliares `palavras.h` e `respostas.h`. O primeiro arquivo contém todas as palavras a serem codificadas em Morse. O segundo arquivo contém as respostas corretas da codificação dessas palavras. Tratam-se de 50 palavras e respostas. Todas essas palavras advém do Hino da Cidade de São Paulo. Esses arquivos são importados em `kernel.c` usando `#include` e suas variáveis são declaradas usando `extern`.

&emsp; Em segundo lugar, foi implementada a introdução do jogo. Essa introdução está nas linhas 386-401. Nela, o kernel exibe o objetivo do jogo, instruções e o requerimento de ter um alfabeto MORSE em mãos. Para tanto, loopa-se no array intro_do_jogo usando a função `kprint` criada por Sreedharan em seu tutorial.

&emsp; A maior parte do jogo porém, está em suas funções principais, as quais reagem às interrupções da CPU, principalmente no que tange às entradas dos teclados. As funções principais são `muda_palavra()`, `afere_resultado()` e `feedback()` que tem a função `decidir_feedback()` como auxiliar. A primeira, exibe na tela a próxima palavra, além de aumentar o indíce de resposta do usuário e exibir sua pontuação atual. A segunda verifica que se a resposta digitada pelo usuário corresponde a resposta do gabarito. A última, exibe um dos 15 feedbacks positivos ou o único feedback negativo possível: "Você tem um dever a cumprir; Consulte sua consciência" em vermelho.

&emsp; Por último, roda-se a função `fim_do_jogo()`. Essa função define com base na pontuação do usuário um dos seguintes cenários:
- Ideal: 5000/5000 pontos;
- Real: entre 100 e 4900 pontos;
- Desastre: 0 pontos.

&emsp; A originalidade do jogo se encontra nas palavras que advém do hino da cidade de São Paulo. Ademais, os feedbacks estão diretamente relacionadas à qualidade e a história dos Estado de São Paulo, cuja cadeira de governador é minha aspiração desde os 13 anos.

## Intruções para rodar

&emsp; Para rodar essa aplicação você precisa estar em um dispositivo Linux ou Mac (caso use WSL, veja se é possível rodar o GRUB nele). Outros requerimentos são, de acordo com Sreedharan (2014):
* An x86 computer (of course) - sugere-se uma máquina virtual como o QEMU
* Linux
* NASM assembler
* gcc
* ld (GNU Linker)
* GRUB

&emsp; Então, após clonar o repositório, cole as seguintes linhas de comando em seu terminal:

```bash
nasm -f elf32 kernel.asm -o kasm.o
gcc -fno-stack-protector -m32 -c kernel.c -o kc.o
ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o
qemu-system-i386 -kernel kernel
```

E divirta-se.

Caso tenha alguma dúvida em como rodar, consulte o seguinte repositório: https://github.com/arjun024/mkeykernel.

## Vídeo

<iframe width="560" height="315" src="https://www.youtube.com/embed/lPT-bcSRxD8?si=Ik5EG1JIrXTzohE9" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>




