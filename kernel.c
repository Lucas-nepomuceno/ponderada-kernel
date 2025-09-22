/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "keyboard_map.h"
#include "palavras.h"
#include "respostas.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define PONTUACAO_MAXIMA 5000

#define ENTER_KEY_CODE 0x1C

extern unsigned char palavras[50][50];
extern unsigned char keyboard_map[128];
extern unsigned char respostas[][50];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;

// variaveis auxiliares
int indice_da_palavra_atual = 0;
int indice_caractere_atual = 0;
int pontuacao = 0;
unsigned char palavra_digitada_pelo_usuario[50];

//variaveis de controle
int acertou = 0;
int fim = 0;


struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void intToStr(int N, char *str) {
    int i = 0;
  
    // Save the copy of the number for sign
    int sign = N;

    // If the number is negative, make it positive
    if (N < 0)
        N = -N;

    // Extract digits from the number and add them to the
    // string
    while (N > 0) {
      
        // Convert integer digit to character and store
      	// it in the str
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 

    // If the number was negative, add a minus sign to the
    // string
    if (sign < 0) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
} // Créditos GeeksforGeeks: https://www.geeksforgeeks.org/c/how-to-convert-an-integer-to-a-string-in-c/

void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x02;
	}
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
}

void decidir_feedback(int i) {
	if (i == 0){
		const char *str = "Viva a Sao Paulo!";
		kprint(str);
	}
	if (i == 1){
		const char *str = "Estado Maravilhoso!";
		kprint(str);
	}
	if (i == 2){
		const char *str = "Vem morar em SP!";
		kprint(str);
	}
	if (i == 3){
		const char *str = "Sempre Paulista!";
		kprint(str);
	}
	if (i == 4){
		const char *str = "Grande....... Estado";
		kprint(str);
	}
	if (i == 5){
		const char *str = "A ponte para o progresso nacional!";
		kprint(str);
	}
	if (i == 6){
		const char *str = "Eh assim que se produz riqueza!";
		kprint(str);
	}
	if (i == 7){
		const char *str = "Sao Paulo hoje. Sao Paulo amanha. Sao Paulo sempre!";
		kprint(str);
	}
	if (i == 8){
		const char *str = "Voce esta tao paulista hoje!";
		kprint(str);
	}
	if (i == 9){
		const char *str = "Cuzcuz? Tem que ser paulista.";
		kprint(str);
	}
	if (i == 10){
		const char *str = "Sao Paulo de pau e pedra";
		kprint(str);
	}
	if (i == 11){
		const char *str = "Sao Paulo de pedra e cal";
		kprint(str);
	}
	if (i == 12){
		const char *str = "Sao Paulo fazendo a ponte do progresso nacional!";
		kprint(str);
	}
	if (i == 13){
		const char *str = "Viva a 32!";
		kprint(str);
	}
	if (i == 14){
		const char *str = "M.M.D.C nunca sera esquecido!";
		kprint(str);
	}
}

void feedback(void) {
	if (acertou) {
		int feedback_adequado = (pontuacao / 100) % 15;
		decidir_feedback(feedback_adequado);
	} else {
		const char *str = "Voce tem um dever a cumprir; Consulte a sua consciencia!";
		unsigned int i = 0;
		while (str[i] != '\0') {
			vidptr[current_loc++] = str[i++];
			vidptr[current_loc++] = 0xc;
		}
	}
	kprint_newline();
	kprint_newline();
}

void muda_a_palavra(void) {
	clear_screen();
	current_loc = 0;
	char str[10];
	feedback();
	if (pontuacao > 0) {
		char *pontuacao_atual_string = "Pontuacao Atual: ";
		kprint(pontuacao_atual_string);
		intToStr(pontuacao, str);
		kprint(str);
	}
	else {
		char *pontuacao_atual_string = "Pontuacao Atual: 0";
		kprint(pontuacao_atual_string);
	}
	char *pontuacao_possivel = "/5000";
	kprint(pontuacao_possivel);
	kprint_newline();
	const char *enunciado = "Represente a seguinte palavra em MORSE: ";
	kprint(enunciado);
	kprint(palavras[indice_da_palavra_atual]);
	kprint_newline();
	kprint_newline();
	indice_da_palavra_atual++;
}

void afere_resultado(void) {
	if (indice_da_palavra_atual > 0 && indice_da_palavra_atual < 49) {
		int palavra_esta_correta = 1;
		const int largura_da_palavra = sizeof(respostas[indice_da_palavra_atual - 1]);
		for (int i = 0; i < largura_da_palavra; i++){			
			if (respostas[indice_da_palavra_atual - 1][i] != palavra_digitada_pelo_usuario[i]) {
				palavra_esta_correta = 0;
			}
		}
		if (palavra_esta_correta == 1) {
			pontuacao = pontuacao + 100;
			acertou = 1;
		} else {
			acertou = 0;
		}
	}
}

void fim_do_jogo(void) {
	clear_screen();
	char str[10];
	if (pontuacao >= PONTUACAO_MAXIMA){
		char *feedback_final = "REALIDADE PARALELA! VOCE VENCEU A REVOLUCAO DE 32 E O BRASIL AGORA SE CHAMA REPUBLICA FEDERATIVA DE SAO PAULO. TODAS AS COMUNICACOES OFICIAIS AGORA SAO DADAS EM MORSE";
		kprint(feedback_final);
		char *parabens = "Parabens, soldado!";
		kprint_newline();
		kprint_newline();
		kprint(parabens);
	}
	else if (pontuacao > 0) {
		char *feedback_final = "Parabens pelo esforco! Voce obteve ";
		kprint(feedback_final);
		intToStr(pontuacao, str);
		kprint(str);
		char *pontuacao_possivel = "/5000";
		kprint(pontuacao_possivel);
		char *getulio = ". Mas o Getulio venceu...";
		kprint(getulio);
	}
	else {
		const char *pontuacao_atual_string = "Voce desertou, soldado!";
		unsigned int i = 0;
		while (pontuacao_atual_string[i] != '\0') {
			vidptr[current_loc++] = pontuacao_atual_string[i++];
			vidptr[current_loc++] = 0xc;
		}
	}
	fim = 1;
}

void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;

		if(keycode == ENTER_KEY_CODE && fim) {
			// reinicializa variaveis auxiliares
			fim = 0;
			indice_da_palavra_atual = 0;
			indice_caractere_atual = 0;
			pontuacao = 0;
		}

		if (keycode == ENTER_KEY_CODE && indice_da_palavra_atual == 49) {
			afere_resultado();
			fim_do_jogo();
		}

		if(keycode == ENTER_KEY_CODE && indice_da_palavra_atual < 49) {
			indice_caractere_atual = 0;
			afere_resultado();
			muda_a_palavra();
			return;
		}
		
		if(keycode == 0xE && current_loc > 5*(BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE)) {
        	vidptr[current_loc--] = '\0';
		  vidptr[current_loc--] = 0x07;
		  vidptr[current_loc] = '\0';
		  palavra_digitada_pelo_usuario[--indice_caractere_atual] = '0';
		  return;
		}
		else if (indice_caractere_atual < 50) {
		  palavra_digitada_pelo_usuario[indice_caractere_atual] = keyboard_map[(unsigned char) keycode];
		  vidptr[current_loc++] = palavra_digitada_pelo_usuario[indice_caractere_atual++];
		  vidptr[current_loc++] = 0x07;
		  return;
		}
	}
}

void kmain(void)
{
	const char *introducao = "MORSE em 1932: Aprendendo Codigo MORSE com o Gov";
	const char *objetivo = " Neste jogo, o seu objetivo eh aprender Codigo MORSE na pratica, representando as palavras indicadas em MORSE.";
	const char *instrucao = " Use . para representar um sinal curto e - para representar um sinal longo";
	const char *requerimento = " Se voce ainda nao sabe as letras em MORSE, eh recomendado ter em maos um ALFABETO EM MORSE";
	const char *revolucao = "Lembre-se, a revolucao depende disso!";
	const char *instrucao_para_iniciar = " Aperte ENTER para iniciar o jogo";

	const char* intro_do_jogo[6] = {introducao, objetivo, instrucao, requerimento, revolucao, instrucao_para_iniciar};

	clear_screen();

	for (int i = 0; i < 6; i++) {
		kprint(intro_do_jogo[i]);
		kprint_newline();
		kprint_newline();
	}

	idt_init();
	kb_init();

	while(1);
}
