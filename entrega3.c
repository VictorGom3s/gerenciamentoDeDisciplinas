#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entrega1.h"
#include "entrega2.h"
#include "entrega21.h"
#include "entrega3.h"
#include "getSO.h"

void menuHistoricoAluno()
{
    char ra[20];
    sprintf(ra,"%ld%s",user->ra,EXT);

    FILE *fp;
    fp = fopen(ra,"w");

    if(fp==NULL)
    {
        printf("\nNao foi possivel abrir o arquivo %s",ra);
    }
    else
    {
        inicializaDiscNotaFalta();   // Inicializa as structs que contem nota,falta,ra e seu respectivo descritor
        addDiscNotaFalta();         // Le o arquivo alunosdisciplinas.txt e adiciona para as structs de cima
        
        fprintf(fp,"Faculdade de Tecnologia – UNICAMP\n\n");
        fprintf(fp,"Relatório de Matrícula\n\n");
        fprintf(fp,"Nome Completo: %s\n",user->nome);
        fprintf(fp,"RA: %ld\n",user->ra);

        menuRendimento(fp); // chama a funcao para calcular os coeficiente de rendimento
        writeDisciplinas(fp);   // chama a funcao para escrever as disciplinas

        fflush(fp);
    }

    fclose(fp);
    printf("\nArquivo %s foi gerado com sucesso!\n",ra);
}

void menuRendimento(FILE *fp)
{
    inicializaCoefRend();     // Inicializa as structs que contem coeficiente de Rendimento e ra e seu respectivo descritor
    addAlunosCoefRend();     // Adiciona os alunos para o vetor de struct
    calcCoefRend();         // Calcula o coeficiente de rendimento para todos os alunos

    quicksortCRend(cRend,0,crDesc->top - 1); // faz o sorte em ordem descendente dos cr
    //printAlunos();    //Mostrar os alunos e seus respectivos cr

    for (register int j = 0; j<crDesc->top;j++)
    {
        if (cRend[j].ra == user->ra)
        {
            fprintf(fp,"Coeficiente de Rendimento: %0.2f\n",cRend[j].cr/10);
            fprintf(fp,"Classificação do aluno na turma : %d de %d\n\n\n",j + 1,crDesc->top);
        }
    }

}

//Funcao para inicializar usando malloc as structs
void inicializaCoefRend()
{

    free(cRend);
    cRend =   calloc(capAlunos,sizeof(cRendimento));

    if(cRend==NULL)
        puts("FALHA NO MALLOC DO cRend");

    free(crDesc);
    crDesc =    calloc(1,sizeof(crDescritor));
    
    if(crDesc==NULL)
        puts("FALHA NO MALLOC DO crDesc");

    crDesc->top = 0;
    crDesc->tam = capAlunos;
}

//Caso haja uma sobrecarga das structs -> realoca += capacidade;
void reallocCoefRend()
{
    crDesc->tam += capAlunos;
    cRend = realloc(cRend, crDesc->tam * sizeof(ntDisciplina));
}

//Funcao para adicionar os alunos a struct que sera usada para calcular os coeficiente de rendimento
void addAlunosCoefRend()
{
    long int ra;
    char buffer[250];
	FILE *fp = fopen(fAlunos,"r");
	
	if (fp==NULL)
	{
        printf("\nNao foi possivel abrir o arquivo %s",fAlunos);
	}
	else
	{	
		while(fscanf(fp,"%ld,%[^\n]",&ra,buffer)!=EOF)
		{
            if (crDesc->top == crDesc->tam)
                reallocCoefRend();
                
			cRend[crDesc->top++].ra = ra;
		}
	}

	fclose(fp);	
}

//Funcao de calculo do coeficiente de rendimento para todos os alunos cadastrados
void calcCoefRend()
{
    char disciplina[10],buffer[250];
    long int ra;
    int creditos,erro;

    for(register int i = 0;i<novaDE->top;i++)
    {
        if (novaNTD[i].nota>=5 && novaNTD[i].faltas<25) //Condicao do pdf somente disciplinas aptas sao computadas
        {
            erro = consultaDisciplina(novaNTD[i].disciplina,buffer,&creditos); // Funcao para voltar os creditos
        
            if (erro==0)
            {
                for (register int j = 0; j<crDesc->top;j++) // Achar a posicao da struct onde esta o respectivo aluno
                {
                    if (cRend[j].ra == novaNTD[i].ra)
                    {
                        cRend[j].credTotal += creditos;     
                        cRend[j].cvTotal += creditos * novaNTD[i].nota;
                        cRend[j].cr = cRend[j].cvTotal/cRend[j].credTotal;
                    }
                }
            }
            else
                continue;   
        }
        else
            continue;
    }
}

void writeDisciplinas(FILE *fp)
{
    //5* Todas as disciplinas que o aluno cursou devem aparecer em ordem semestral
    sortMatriculas(0); //Deixar os semestres && RAs em ordem crescente
    
    //printArray(); Verificacao da struct depois do sort

    if(fp==NULL)
    {
        printf("\nNao foi possivel continuar a utilizar o arquivo RAdoAluno.txt");
    }
    else
    {
        fprintf(fp,"Disciplina\tNota\t\tFaltas (%%)\tSituação\n");

        for(register int i = 0;i<novaDE->top;i++)
        {

            if(user->ra == novaNTD[i].ra) // Se a disciplina estiver matriculado ao aluno logado
            {

                //fprintf(fp,"\n\nSI200  		    7.1 	    10.4    	  Aprovado por Nota e Frequência\n"); //Exemplo de formatacao para pegar a tabulacao
                //if(strcmp(EXT,".txt")==0)
                //    fprintf(fp,"%s  		    %0.2f 	    %0.2f    	  ",novaNTD[i].disciplina,novaNTD[i].nota,novaNTD[i].faltas);
                //else
                fprintf(fp,"%s\t\t%0.2f\t\t%0.2f\t\t",novaNTD[i].disciplina,novaNTD[i].nota,novaNTD[i].faltas);

                //Verifica os parametros de nota e falta para verificar se o aluno passou ou nao
                /*  Condicoes:
                3* Caso o número de faltas seja igual ou superior a 25%, o aluno deve ser reprovado por falta
                4* Caso o aluno possua nota abaixo de 5, o mesmo deve ser considerado reprovado por nota
                */
                if(novaNTD[i].nota >= 5 && novaNTD[i].faltas < 25)              
                    fprintf(fp,"Aprovado por Nota e Frequência\n");
                else if(novaNTD[i].nota < 5 && novaNTD[i].faltas < 25)
                    fprintf(fp,"Reprovado por Nota\n");
                else if(novaNTD[i].nota >= 5 && novaNTD[i].faltas >= 25)
                    fprintf(fp,"Reprovado por Faltas\n");
                else
                    fprintf(fp,"Reprovado por Nota e Faltas\n");

                fflush(fp);
            }
        }
    }
}

// Funcao para printar o vetor dos alunos com notas, ra, faltas. Usada para verificacao
void printArray()
{
    for(register int i = 0;i<novaDE->top;i++)
    {
        printf("%ld,%s,%d,%0.2f,%0.2f\n",novaNTD[i].ra,novaNTD[i].disciplina,novaNTD[i].semestre,novaNTD[i].nota,novaNTD[i].faltas);
    }
}

//Funcao que troca a posicao da struct dentro do arranjo
void swapNTDisc(ntDisciplina *a, ntDisciplina *b)
{
    ntDisciplina temp = *a;
    *a = *b;
    *b = temp;
}

//quicksort usado para ordenar o array de registros dos alunos por semestre
void quicksortNTDisc(ntDisciplina *arr, int l, int r)
{

    if (l >= r)
    {
        return;
    }

    int pivot = arr[r].semestre;
    int cnt = l;

    for (int i = l; i <= r; i++)
    {
        if (arr[i].semestre <= pivot)
        {
            swapNTDisc(&arr[cnt], &arr[i]);            
            cnt++;
        }
    }

    quicksortNTDisc(arr, l, cnt-2);
    quicksortNTDisc(arr, cnt, r);
}

//quicksort usado para ordenar o array de registros dos alunos por RA
void quicksortNTDiscRA(ntDisciplina *arr, int l, int r)
{
    
    if (l >= r)
    {
        return;
    }

    long int pivot = arr[r].ra;
    int cnt = l;

    for (int i = l; i <= r; i++)
    {
        if (arr[i].ra <= pivot)
        {
            swapNTDisc(&arr[cnt], &arr[i]);            
            cnt++;
        }
    }

    quicksortNTDiscRA(arr, l, cnt-2);
    quicksortNTDiscRA(arr, cnt, r);
}

// funcao para printar os valores de cr dos alunos. Usado para verificacao
void printAlunos()
{
    for(int i = 0;i<crDesc->top;i++)
        printf("RA: %ld         cred*nota:%0.2f         creditos:%0.2f cr:%0.2f\n",cRend[i].ra,cRend[i].cvTotal,cRend[i].credTotal,cRend[i].cr);
}

//Funcao que troca a posicao da struct dentro do arranjo
void swapCRend(cRendimento *a, cRendimento *b)
{
    cRendimento temp = *a;
    *a = *b;
    *b = temp;
}

// Quicksort usado para ordenar o array de registros dos alunos por cr em ordem decrescente
void quicksortCRend(cRendimento *arr, int l, int r)
{
    if (l >= r)
    {
        return;
    }

    float pivot = arr[r].cr;
    int cnt = l;

    for (int i = l; i <= r; i++)
    {
        if (arr[i].cr >= pivot)
        {
            swapCRend(&arr[cnt], &arr[i]);           
            cnt++;
        }
    }

    quicksortCRend(arr, l, cnt-2);
    quicksortCRend(arr, cnt, r);
}

void sortMatriculas(int op)
{
    if(op==1)
    {
        inicializaDiscNotaFalta();  //Inicializa ambas as structs
        addDiscNotaFalta();         //Le o arquivo e adiciona para as structs
    }

   	quicksortNTDisc(novaNTD,0,novaDE->top - 1);
	
    int last = 0;
    for (register int i = 0;i<novaDE->top;i++)
    {
        if(novaNTD[i].semestre != novaNTD[i+1].semestre)
        {
            quicksortNTDiscRA(novaNTD,last,i);
            last = i+1;
        }

    }

    if(op==1)
        persisteDisciplinaStruct();
}