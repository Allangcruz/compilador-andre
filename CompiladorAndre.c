#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int TOTAL_CONSUMO_MEMORIA = 0;
int MAX_TOTAL_CONSUMO_MEMORIA = 256000;

// Structs de lista encadeada
typedef struct simbolo {
    char nomeVariavel[1000];
    char valorVariavel[1000];
	char tipagemVariavel[1000];
} Simbolo;

typedef struct elemSimbolo {
    Simbolo item;
    struct elemSimbolo *prox;
} ElemSimbolo;

typedef struct elemSimbolo* TabelaSimbolo;

void limparLixoString(char vetor[]);
void removerTabulacao(char* palavra);
void removerQuebraLinha(char* palavra);
void removerCaracterEspaco(char *palavra);
void removePalavrasComAspas(char *palavra, char *palavraComAspas);
int isLinhaVazia(char *palavra);
void verificarAnalises(FILE *arquivo, TabelaSimbolo* simbolos);
void verificarLiterais(char *palavra, int nuLinha);
int verificarCondicaoParada(int ascii);
int verificarCaracteresEspeciaisValidos(int ascii);
int verificarPontoVingula(char *palavra, int nuLinha);
int verificarDeclaracaoVariavel(char *palavra, int nuLinha);
int verificarMainValido(char *palavra, int nuLinha);
int verificarPalavrasReservadas(char *palavra);
int verificarTipoVariavel(char *palavra);
void mostrarConsumoMemoria();
void mostrarErrorValidacao(int nuLinha, int tipoErro, char *palavra);
void salvarMemoriaConsumida(int memoria, int situacao);
TabelaSimbolo* criaListaTabelaSimbolo();
int insereFinalTabelaSimbolo(TabelaSimbolo* lista, Simbolo simbolo);
int tamanhoTabelaSimbolo(TabelaSimbolo* lista);
int isVazioTabelaSimbolo(TabelaSimbolo* lista);
void imprimeTabelaSimbolo(TabelaSimbolo* lista);
void atualizarValorVariavel(TabelaSimbolo* simbolos, char *variavel, char *valor);
int verificarExistenciaVariavel(char *variavel, TabelaSimbolo* simbolos);

/**
 * Executando compilador
 */
int main () {
	printf("COMPILADORES DO ANDRE SILVA FERNANDES\n");
	printf("*******************************************************************\n");

	char nomeArquivo[] = {"ValidarCompiladorTeste.txt"};
	FILE *arquivo;
	TabelaSimbolo* simbolos = criaListaTabelaSimbolo();

    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
    	printf("\nFAVOR VERIFIQUE SE O AQUIVO EXISTE.\n");
        exit(0);
	}

	salvarMemoriaConsumida(sizeof(nomeArquivo), 1);
	salvarMemoriaConsumida(sizeof(arquivo), 1);
	salvarMemoriaConsumida(sizeof(simbolos), 1);

	verificarAnalises(arquivo, simbolos);

	// exibir dados na tabela de simbolos
	imprimeTabelaSimbolo(simbolos);

    mostrarConsumoMemoria();
}

// ************************************************************************************************************************
// Implementacoes das assinaturas
// ************************************************************************************************************************

/**
 * Verifica se a variavel ja existe na tabela de simbolos.
 */
int verificarExistenciaVariavel(char *variavel, TabelaSimbolo* simbolos) {
	int isValido = 0;

	if (simbolos == NULL) {
        return 0;
    }

	ElemSimbolo* no = *simbolos;

    while (no != NULL) {
    	if (strcmp(variavel, no->item.nomeVariavel) == 0) {
    		isValido = 1;
    		break;
		}
        no = no->prox;
    }

	return isValido;
}

/**
 * Remove conteudo com aspas da linha.
 */
void removePalavrasComAspas(char *palavra, char *palavraComAspas) {

	int i, valorAscii, countPalavra = 0, countPalavraValida = 0, isPossuiAspas = 0;

	char palavraComConteudoAspas[1000];
	char palavraSemConteudoAspas[1000];

	limparLixoString(palavraComConteudoAspas);
	limparLixoString(palavraSemConteudoAspas);
	limparLixoString(palavraComAspas);

	for (i = 0; i < strlen(palavra); i++) {

		valorAscii = (int) palavra[i];

		if (isPossuiAspas == 0 && valorAscii == 34) {
			isPossuiAspas = 1;
			palavraComConteudoAspas[countPalavra] = palavra[i];
			countPalavra++;
			continue;
		}

		if (isPossuiAspas > 0 && valorAscii != 34) {
			palavraComConteudoAspas[countPalavra] = palavra[i];
			countPalavra++;
			continue;
		}

		if (isPossuiAspas == 1 && valorAscii == 34) {
			palavraComConteudoAspas[countPalavra] = palavra[i];
			countPalavra++;
			isPossuiAspas = 0;
			continue;
		}

		if ((isPossuiAspas == 0)) {
			palavraSemConteudoAspas[countPalavraValida] = palavra[i];
			countPalavraValida++;
		}
	}

	strcpy(palavra, palavraSemConteudoAspas);
	strcpy(palavraComAspas, palavraComConteudoAspas);
}

/**
 * Verifica se a declaracao e variavel é valida.
 */
int verificarDeclaracaoVariavel(char *palavra, int nuLinha) {
    int i, ascii, primeiroAscii;

	ascii = (int) palavra[0];

    if (strlen(palavra) == 0) {
        return 0;
    }
	if (ascii == 35) {
        primeiroAscii =(int)palavra[1];
		if (!(primeiroAscii >= 97 && primeiroAscii <= 122)) {
			mostrarErrorValidacao(nuLinha, 5, palavra);
		}

		for (i = 2; i < strlen(palavra); i++) {
			ascii = (int) palavra[i];

			if (! (
                (ascii >= 97 && ascii <= 122) ||
                (ascii >= 48 && ascii <= 57) ||
                (ascii >= 65 && ascii <= 90) ||
                (ascii == 91 || ascii == 93 || ascii == 46))
            )
            {
			    mostrarErrorValidacao(nuLinha, 5, palavra);
			}
		}
	} else {
        return 0;
    }

	return 1;
}

/**
 * Verifica se a declaracao da palavra reservada 'main' esta valida
 */
int verificarMainValido(char *palavra, int nuLinha) {
	int i, ascii, count = 0, isInValido = 0;
	char palavraTmp[1000];
	char palavraProcessada[1000];
	limparLixoString(palavraTmp);
	limparLixoString(palavraProcessada);

	strcpy(palavraProcessada, palavra);

	removerQuebraLinha(palavraProcessada);
	removerTabulacao(palavraProcessada);
	removerCaracterEspaco(palavraProcessada);

	for (i = 0; i < strlen(palavraProcessada); i++) {
		ascii = (int) palavraProcessada[i];
		palavraTmp[count] = (char) ascii;
		count++;

		if (strcmp(palavraTmp, "main(") == 0) {
			isInValido = 1;
			break;
		}
	}

	if (!isInValido) {
		mostrarErrorValidacao(nuLinha, 2, palavra);
	}

	isInValido = 0;
	count = 0;
	limparLixoString(palavraTmp);

	for (i = 0; i < strlen(palavraProcessada); i++) {
		ascii = (int) palavraProcessada[i];
		palavraTmp[count] = (char) ascii;
		count++;

		if (strcmp(palavraTmp, "main(){") == 0) {
			isInValido = 1;
			break;
		}
	}

	if (!isInValido) {
		mostrarErrorValidacao(nuLinha, 3, palavra);
	}

	return 1;
}

/**
 * Remove caracter 'espaco' da string informada.
 */
void removerCaracterEspaco(char *palavra) {
	int i, ascii, count = 0;
	char palavraTmp[1000];
	limparLixoString(palavraTmp);

	for (i = 0; i < strlen(palavra); i++) {
		ascii = (int) palavra[i];
		// Espaco => 32
		if (ascii != 32) {
			palavraTmp[count] = palavra[i];
			count++;
		}
	}

	strcpy(palavra, palavraTmp);
}

/**
 * Verifica se existe ponto e virgula no final da linha
 */
int verificarPontoVingula(char *palavra, int nuLinha) {
	char palavraProcessada[1000];
	limparLixoString(palavraProcessada);

	strcpy(palavraProcessada, palavra);

	removerQuebraLinha(palavraProcessada);
	removerCaracterEspaco(palavraProcessada);

	int i, ascii;
	int tamanhoPalavra = strlen(palavraProcessada) - 1;
	int tamanhoPalavraPosicao = tamanhoPalavra;
	int quantidadeVezesTestarCondicao = 0;

	for (i = tamanhoPalavra; i >= 0; i--) {
		ascii = palavraProcessada[i];

		if (ascii == 59 && i == tamanhoPalavraPosicao) {
			return 1;
		}

		if (ascii == 59 && i == tamanhoPalavraPosicao) {
			return 1;
		}

		if (quantidadeVezesTestarCondicao > 2) {
			return 0;
		}

		quantidadeVezesTestarCondicao++;
		tamanhoPalavraPosicao--;
	}
	return 0;
}

/**
 * Verifica se é um tipo de variavel valido
 */
int verificarTipoVariavel(char *palavra) {
	if (strcmp(palavra, "string") == 0) {
		return 1;
	}

	if (strcmp(palavra, "decimal") == 0) {
		return 1;
	}

	if (strcmp(palavra, "integer") == 0) {
		return 1;
	}

	return 0;
}

/**
 * Verifica se o token formado é alguma palavra reservada.
 */
int verificarPalavrasReservadas(char *palavra) {
	int i;
	char reservadas[9][8] = {"main", "puts", "gets", "if", "else", "for", "integer", "string", "decimal"};

    for (i = 0; i < 9; i++) {
		if (strcmp(palavra, reservadas[i]) == 0) {
			return 1;
		}
	}
    return 0;
}

/**
 * Limpar o conteudo da string, quando a variavel for nova.
 */
void limparLixoString(char vetor[]) {
	int i = 0;

	while (i < 1000) {
		vetor[i] = '\0';
		i ++;
	}
}

/**
 * Remove todos os caractes referente a quebra de linha.
 */
void removerQuebraLinha(char* palavra) {
	int i, valorAscii, count = 0;
	char palavraAux[1000];
	limparLixoString(palavraAux);

	for (i = 0; i < strlen(palavra); i++) {
		valorAscii = (int) palavra[i];

		if ((valorAscii != 13) && (valorAscii != 10)) {
			palavraAux[count] = palavra[i];
			count++;
		}
	}

	strcpy(palavra, palavraAux);

	salvarMemoriaConsumida(sizeof(i), 1);
	salvarMemoriaConsumida(sizeof(valorAscii), 1);
	salvarMemoriaConsumida(sizeof(count), 1);
	salvarMemoriaConsumida(sizeof(palavraAux), 1);
}

/**
 * Exibe a mensagem de error
 */
void mostrarErrorValidacao(int nuLinha, int tipoErro, char *palavra) {

	if (tipoErro == 0) {
		printf("EXISTE CARACTER QUE NAO PERTENCE A TABELA DE LITERAIS. (%s) - [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 1) {
		printf("TOKEN FORMADO INVALIDO. (%s) - [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 2) {
		printf("PALAVRA 'main()' ESTA INCORRETA, FAVOR VERIFICAR ABERTURA DE PARENTES. (%s) - [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 3) {
		printf("PALAVRA 'main(){' ESTA INCORRETA, FAVOR VERIFICAR ABERTURA DE CHAVES OU REMOVA OS PARAMETROS. (%s)- [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 4) {
		printf("TODA DECLARACAO DE VARIAVEL E OBRIGATORIO TER ';'. (%s)- [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 5) {
		printf("A FORMACAO DE TOKEN DE VARIAVEL ESTA INVALIDA. (%s)- [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 6) {
		printf("VARIAVEL (%s) JA FOI DECLARADA ANTERIORMENTE. [LINHA => %i].\n", palavra, nuLinha);
	} else if (tipoErro == 7) {
		printf("VARIAVEL (%s) NAO DECLARADA. [LINHA => %i].\n", palavra, nuLinha);
	} else {
		printf("\nERROR NAO EXISTE.\n");
	}
	printf("\n");
	mostrarConsumoMemoria();
	exit(0);
}

/**
 * Salva o consumo de memoria, para apresentar no final
 */
void salvarMemoriaConsumida(int memoria, int situacao) {
	if (situacao == 1) {
		TOTAL_CONSUMO_MEMORIA = TOTAL_CONSUMO_MEMORIA + memoria;
	} else {
		TOTAL_CONSUMO_MEMORIA = TOTAL_CONSUMO_MEMORIA - memoria;
	}

	float porcentagem = 0;
	if (MAX_TOTAL_CONSUMO_MEMORIA > 0) {
		porcentagem = (TOTAL_CONSUMO_MEMORIA * 100) / MAX_TOTAL_CONSUMO_MEMORIA;
	}

	if (porcentagem > 90 && porcentagem < 99) {
		printf("A MEMORIA ATUAL ESTA ENTRE 90 %% A 99 %%, MEMORIA ATUAL: %.2f %%\n\n", porcentagem);
	}

	if (TOTAL_CONSUMO_MEMORIA > MAX_TOTAL_CONSUMO_MEMORIA) {
		printf ("MEMORIA ATINGIU O LIMITE PARAMETRIZADO.\n");
		mostrarConsumoMemoria();
		exit(0);
	}
}

/**
 * Exibe o consumo de memoria.
 */
void mostrarConsumoMemoria() {
	printf("\n---------------------------------------------------------------------\n");
	printf("\nMEMORIA PARAMETRIZADA: %d bytes\n", TOTAL_CONSUMO_MEMORIA);
	float porcentagem = 0;
	if (MAX_TOTAL_CONSUMO_MEMORIA > 0) {
		porcentagem = (TOTAL_CONSUMO_MEMORIA * 100) / MAX_TOTAL_CONSUMO_MEMORIA;
	}

	printf("PORCENTAGEM DE MEMORIA => %.2f %% de %i bytes\n\n" , porcentagem, MAX_TOTAL_CONSUMO_MEMORIA);
}

/**
 * Verifica se a linha esta vazia.
 */
int isLinhaVazia(char *palavra) {
    int i, ascii;

    for (i = 0; i < strlen(palavra); i++) {
        ascii = (int) palavra[i];
        if (ascii != 32 && ascii != 9 && ascii != 10 && ascii != 13) {
            return 0;
        }
    }

    return 1;
}

/**
 * Remove caracteres de tabulação.
 */
void removerTabulacao(char* palavra) {
	int i, valorAscii, count = 0;
	char palavraAux[1000];
	limparLixoString(palavraAux);

	for (i = 0; i < strlen(palavra); i++) {
		valorAscii = (int) palavra[i];
		// Tab => 9
		if (valorAscii != 9) {
			palavraAux[count] = palavra[i];
			count++;
		}
	}

	strcpy(palavra, palavraAux);

	salvarMemoriaConsumida(sizeof(i), 1);
	salvarMemoriaConsumida(sizeof(valorAscii), 1);
	salvarMemoriaConsumida(sizeof(count), 1);
	salvarMemoriaConsumida(sizeof(palavraAux), 1);
}

/**
 * Verifica se o caracter informado é uma condicao de parada
 */
int verificarCondicaoParada(int ascii) {
    if ((ascii != 35) && (ascii != 10) && (ascii != 9) && (ascii != 32) && (ascii != 40) && (ascii != 41) && (ascii != 59) && (ascii != 123) && (ascii != 125) && (ascii != 44) && (ascii != 33) && (ascii != 34) && (ascii != 60) && (ascii != 61) && (ascii != 62) && (ascii != 43) && (ascii != 10)) {
        return 0;
    }
    return 1;
}

/**
 * Verifica se o caracter informador pertecem a tabela de literais da gramatica
 */
int verificarCaracteresEspeciaisValidos(int ascii) {
    if ((ascii != 0) && (ascii != 35) && (ascii != 46) && (ascii != 10) && (ascii != 9) && (ascii != 32) && (ascii != 40) && (ascii != 41) && (ascii != 91) && (ascii != 93) && (ascii != 94) && (ascii != 95) && (ascii != 59) && (ascii != 123) && (ascii != 125) && (ascii != 44) && (ascii != 33) && (ascii != 34) && (ascii != 60) && (ascii != 61) && (ascii != 62) && (ascii != 43) && (ascii != 45) && (ascii != 42) && (ascii != 47) && (ascii != 37) && (ascii != 10)) {
        return 0;
    }
    return 1;
}

void verificarLiterais(char *palavra, int nuLinha) {
	int i = 0, ascii;

	for (i = 0; i <= strlen(palavra); i++) {
		ascii = (int) palavra[i];

		// numeros (0-9) && alfabeto a-z, A-Z
		// printf("Debug => (%s) - (%i) - (%i) - (%c) - (%i) - (%i) - (%i)\n", palavra, nuLinha, ascii, (char) ascii, (ascii >= 48 && ascii <= 57), ((ascii >= 65 && ascii <= 90) && (ascii >= 97 && ascii <= 122)), verificarCaracteresEspeciaisValidos(ascii));
		if (! (ascii >= 48 && ascii <= 57) && !((ascii >= 65 && ascii <= 90) || (ascii >= 97 && ascii <= 122)) && (verificarCaracteresEspeciaisValidos(ascii) == 0)) {
			printf("\n(%c) NAO É LITERAIS VALIDO.\n", palavra[i]);
			mostrarErrorValidacao(nuLinha, 0, palavra);
			exit(0);
		}
	}
}

// ------------------------------------------------------------------------------------
// FUNCOES DE LISTA ENCADEADA (https://programacaodescomplicada.wordpress.com/complementar/)
TabelaSimbolo* criaListaTabelaSimbolo() {
    TabelaSimbolo* lista = (TabelaSimbolo*) malloc(sizeof(TabelaSimbolo));

    if (lista != NULL) {
        *lista = NULL;
    }

    return lista;
}

void liberaListaTabelaSimbolo(TabelaSimbolo* lista) {
    if (lista != NULL) {
        ElemSimbolo* no;

        while ((*lista) != NULL) {
            no = *lista;
            *lista = (*lista)->prox;
            free(no);
        }

        free(lista);
    }
}

int insereFinalTabelaSimbolo(TabelaSimbolo* lista, Simbolo simbolo) {
    if (lista == NULL) {
        return 0;
    }

    ElemSimbolo *no;
    no = (ElemSimbolo*) malloc(sizeof(ElemSimbolo));
    if (no == NULL) {
        return 0;
    }

    no->item = simbolo;
    no->prox = NULL;

    // lista vazia: insere inicio
    if ((*lista) == NULL) {
        *lista = no;
    } else {
        ElemSimbolo *aux;
        aux = *lista;

        while (aux->prox != NULL) {
            aux = aux->prox;
        }

        aux->prox = no;
    }
    return 1;
}

int tamanhoTabelaSimbolo(TabelaSimbolo* lista) {
    if (lista == NULL) {
        return 0;
    }

    int cont = 0;
    ElemSimbolo* no = *lista;

    while (no != NULL) {
        cont++;
        no = no->prox;
    }

    return cont;
}

int isVazioTabelaSimbolo(TabelaSimbolo* lista) {
	int isVazio = 0;

    if (lista == NULL) {
        isVazio = 1;
    }

    if (*lista == NULL) {
        isVazio = 1;
    }

    return isVazio;
}

void imprimeTabelaSimbolo(TabelaSimbolo* lista) {
    if (lista == NULL) {
        return;
    }

    printf("********************************************************************\n");
    printf("TABELA SIMBOLOS \n");
    printf("********************************************************************\n");

    ElemSimbolo* no = *lista;

    if (isVazioTabelaSimbolo(lista)) {
    	printf("TABELA DE SIMBOLOS VAZIA.\n");
    	printf("********************************************************************\n\n");
	}

    while (no != NULL) {
        printf("TIPAGEM: %s \n", no->item.tipagemVariavel);
        printf("NOME VARIAVEL: %s \n", no->item.nomeVariavel);
        printf("********************************************************************\n");
        no = no->prox;
    }
}

// Valida todas as regras
void verificarAnalises(FILE *arquivo, TabelaSimbolo* simbolos) {
	int i = 0;
    char palavraAuxiliar[1000];
    char conteudoLinha[1000];
	char nomeTipoVariavel[1000];
    char conteudoPorCaracter[1000];
	char conteudoLinhaComAspas[1000];
	char nomeVariavel[1000];

	int nuLinha = 0;
    int ascii;
    int tamanhoLinha = 0;
	int count = 0;
	int isMainExistente = 0;
	int isVariavelValida = 0;
	int isPossuiDeclaracaoVariavel = 0;
	int isReservadaValida = 0;
	int countVariaveis = 0;

	// Limpando lixo de memoria.
    limparLixoString(palavraAuxiliar);
    limparLixoString(conteudoLinha);
	limparLixoString(nomeTipoVariavel);
	limparLixoString(conteudoPorCaracter);
	limparLixoString(conteudoLinhaComAspas);
	limparLixoString(nomeVariavel);

	salvarMemoriaConsumida(sizeof(palavraAuxiliar), 1);
	salvarMemoriaConsumida(sizeof(conteudoLinha), 1);
	salvarMemoriaConsumida(sizeof(nomeTipoVariavel), 1);
	salvarMemoriaConsumida(sizeof(conteudoPorCaracter), 1);
	salvarMemoriaConsumida(sizeof(conteudoLinhaComAspas), 1);
	salvarMemoriaConsumida(sizeof(nomeVariavel), 1);
	salvarMemoriaConsumida(sizeof(nuLinha), 1);
	salvarMemoriaConsumida(sizeof(ascii), 1);
	salvarMemoriaConsumida(sizeof(tamanhoLinha), 1);
	salvarMemoriaConsumida(sizeof(count), 1);
	salvarMemoriaConsumida(sizeof(isMainExistente), 1);
	salvarMemoriaConsumida(sizeof(isVariavelValida), 1);
	salvarMemoriaConsumida(sizeof(isPossuiDeclaracaoVariavel), 1);
	salvarMemoriaConsumida(sizeof(isReservadaValida), 1);
	salvarMemoriaConsumida(sizeof(countVariaveis), 1);

	if (arquivo != NULL) {
		while ((fgets(conteudoLinha, sizeof(conteudoLinha), arquivo)) != NULL) {
			nuLinha ++;
			strcpy(conteudoPorCaracter, conteudoLinha);

			// tratamento da linha
			removerTabulacao(conteudoPorCaracter);
			removePalavrasComAspas(conteudoPorCaracter, conteudoLinhaComAspas);
			removerQuebraLinha(conteudoPorCaracter);

			// Valida caracteres invalidos
			verificarLiterais(conteudoPorCaracter, nuLinha);

			tamanhoLinha = strlen(conteudoPorCaracter);

			if (tamanhoLinha > 0 && isLinhaVazia(conteudoPorCaracter) == 0) {

				for (i = 0; i < tamanhoLinha; i++) {
					ascii = (int) conteudoPorCaracter[i];

					if (i == (tamanhoLinha -1) && !verificarCondicaoParada(ascii)) {
						palavraAuxiliar[count] = (char) ascii;
						count++;
					}

					if (! verificarCondicaoParada(ascii) && (i != (tamanhoLinha -1))) {
						palavraAuxiliar[count] = (char) ascii;
						count++;
					} else {

						isVariavelValida = verificarDeclaracaoVariavel(palavraAuxiliar, nuLinha);
						if (isVariavelValida) {
							countVariaveis ++;
							strcpy(nomeVariavel, palavraAuxiliar);
						}

						isReservadaValida = verificarPalavrasReservadas(palavraAuxiliar);
						if (isReservadaValida) {
							if (verificarTipoVariavel(palavraAuxiliar) == 1) {
								isPossuiDeclaracaoVariavel = 1;

								if (! verificarPontoVingula(conteudoPorCaracter, nuLinha)) {
									mostrarErrorValidacao(nuLinha, 4, conteudoLinha);
								}

								strcpy(nomeTipoVariavel, palavraAuxiliar);
							}

							if (strcmp(palavraAuxiliar, "main") == 0) {
								isMainExistente ++;
								verificarMainValido(conteudoPorCaracter, nuLinha);
							}
						}

						if (strlen(palavraAuxiliar) > 0) {
							if (!isVariavelValida && !isReservadaValida) {
								mostrarErrorValidacao(nuLinha, 1, palavraAuxiliar);
							} else {
								if (isPossuiDeclaracaoVariavel && isVariavelValida) {
									Simbolo simbolo;
									strcpy(simbolo.valorVariavel, "");
								    strcpy(nomeVariavel, palavraAuxiliar);

									if (isVariavelValida) {
										// validar se a variavel ja foi declarada anteriormente.
										if (verificarExistenciaVariavel(nomeVariavel, simbolos) == 1) {
											mostrarErrorValidacao(nuLinha, 6, nomeVariavel);
										}
									}

								    strcpy(simbolo.tipagemVariavel, nomeTipoVariavel);
								    strcpy(simbolo.nomeVariavel, nomeVariavel);
								    insereFinalTabelaSimbolo(simbolos, simbolo);
								}

								if (isVariavelValida) {
									// validar se a variavel não foi declarada, pelo tipo
									if (verificarExistenciaVariavel(nomeVariavel, simbolos) == 0) {
										mostrarErrorValidacao(nuLinha, 7, nomeVariavel);
									}
								}
							}
						}

						limparLixoString(palavraAuxiliar);
						count = 0;

					} // fim else

					// Verifica se o valor inicializa o nome de uma possivel variavel 35 => #
					if (ascii == 35) {
						palavraAuxiliar[count] = (char) ascii;
						count++;
					}
				}

				// limpar para a proxima linha
				count = 0;
				countVariaveis = 0;
				isReservadaValida = 0;
				isVariavelValida = 0;
				isPossuiDeclaracaoVariavel = 0;
				limparLixoString(palavraAuxiliar);
				limparLixoString(nomeVariavel);
				limparLixoString(nomeTipoVariavel);
			}
		} // fim else

		if (!isMainExistente) {
			printf("FUNCAO 'main' É OBRIGATORIO.\n");
			exit(0);
		}
	}
}


