#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entrega1.h"
#include "entrega2.h"
#include "entrega21.h"

void menuAlterarNotaFalta()
{
    
    int  semestre,erro,hasDisciplina;
    float nota,falta;
    char disciplina[10];

    inicializaDiscNotaFalta();  //Inicializa ambas as structs
    addDiscNotaFalta();         //Le o arquivo e adiciona para as structs

    puts("\nMenu alterar nota e falta\n");
    puts("Para sair, digite 0 ou valores negativos");
    puts("Para sair, digite XX000");
    
    do // do enquanto n mostrar disciplinas
    {
        printf("Digite o semestre: ");
        scanf("%d",&semestre);
        getchar();

        if(semestre<=0)
        {
            puts("\nTransacao suspensa com sucesso");
            break;
        }

        if (semestre > 10)
        {
            puts("\nSemestre invalido: fora dos valores permitidos para a matricula\n");
            continue;
        }
        else
        {
            puts("\nDisciplinas:");

            showDisciplinas(semestre,&hasDisciplina);  //Mostra as disciplinas relativa ao semestre e usuario logado

            if(hasDisciplina==0)
                printf("Aluno nao matriculado em nenhuma disciplina para o semestre selecionado\n\n");
        }

    }while(hasDisciplina==0 || (semestre <= 0 || semestre > 10));

    if (semestre > 0 && semestre < 11)
    {
    
        printf("\nDigite o codigo da disciplina que deseja fazer a alteracao: ");

        fgets(disciplina,10,stdin);
        limpaChar(disciplina);
        upperChar(disciplina);

        if (strcmp(disciplina,"XX000")!=0)
        {

            erro = verificaDisciplinaStruct(disciplina,semestre);

            if (erro == 0)
            {
        
                do
                {
                    printf("\nNota: ");
                    scanf("%f",&nota);
                    getchar();    

                    if (nota<0 || nota>10)
                        puts("\nValor inserido para nota invalida!");

                }while(nota<0 || nota>10);

                do
                {
                    printf("\nFalta (%%): ");
                    scanf("%f",&falta);
                    getchar();    

                    if (falta<0 || falta>100)
                        puts("\nValor inserido para falta invalida!");

                }while(falta<0 || falta>100);

                modifyDisciplinaStruct(disciplina,semestre,nota,falta);
                persisteDisciplinaStruct();

                puts("\nAtualizacao realizada com sucesso!");
            }
            else
                puts("\nDisciplina nao existente no semestre!");
        }
    }    
}

//Funcao para jogar toda a struct no arquivo
void persisteDisciplinaStruct()
{
    FILE *fp = fopen(fAlunosD,"w");

    if (fp==NULL)
    {
        puts("\nNao foi possivel encontrar o arquivo AlunosDisciplinas");
    }
    else
    {
        //RA,CódigodaDisciplina,Semestre,Nota,Faltas
        for(register int i = 0;i<novaDE->top;i++)
        {
            fprintf(fp,"%ld,%s,%d,%0.2f,%0.2f\n",novaNTD[i].ra,novaNTD[i].disciplina,novaNTD[i].semestre,novaNTD[i].nota,novaNTD[i].faltas);
            fflush(fp);
        }

    }
    
    fclose(fp);
}

//Funcao que modifica para o usuario logado as respectivas notas e faltas da disciplina e semestre
void modifyDisciplinaStruct(char *disciplina,int semestre,float nota,float falta)
{
    for(register int i = 0;i<novaDE->top;i++)
    {
        if (strcmp(disciplina,novaNTD[i].disciplina)==0 && novaNTD[i].semestre==semestre && user->ra == novaNTD[i].ra)
        {
            novaNTD[i].nota = nota;
            novaNTD[i].faltas = falta;
            break;
        }
    }
}

//Funcao que verifica se existe a disciplina dentro do semestre escolhido para o usuario logado
int verificaDisciplinaStruct(char *disciplina,int semestre)
{
        for(register int i = 0;i<novaDE->top;i++)
        {
            if (strcmp(disciplina,novaNTD[i].disciplina)==0 && novaNTD[i].semestre==semestre && user->ra == novaNTD[i].ra)
            {
                return 0;
            }
        }

    return 1;
}

//Funcao para inicializar usando malloc as structs
void inicializaDiscNotaFalta()
{
    free(novaNTD);
    novaNTD =   calloc(cap,sizeof(ntDisciplina));

    if(novaNTD==NULL)
        puts("FALHA NO MALLOC DO novaNTD");

    free(novaDE);
    novaDE =    calloc(1,sizeof(ntDescritor));
    
    if(novaDE==NULL)
        puts("FALHA NO MALLOC DO novaDE");

    novaDE->top = 0;
    novaDE->tam = cap;
}

//Caso haja uma sobrecarga das structs -> realoca += capacidade;
void reallocDiscNotaFalta()
{
    novaDE->tam += cap;
    novaNTD = realloc(novaNTD, novaDE->tam * sizeof(ntDisciplina));
}

//Funcao de leitura do arquivo de notas e faltas e organizando na struct
void addDiscNotaFalta()
{
    
    long int Ra;
    char idDisciplina[10];
    int auxSem;
    float Notas,Faltas;

    FILE *fp = fopen(fAlunosD,"r");

    if (fp==NULL)
    {
        fclose(fp);
        fp = fopen(fAlunosD,"w");
        fclose(fp);
    }
    else
    {
        //RA,CódigodaDisciplina,Semestre,Nota,Faltas
        while(fscanf(fp,"%ld,%[^,],%d,%f,%f\n",&Ra,idDisciplina,&auxSem,&Notas,&Faltas)!=EOF)
        {

            if  (novaDE->top == novaDE->tam)
            {
                reallocDiscNotaFalta();
            }         

            novaNTD[novaDE->top].ra = Ra;            
            novaNTD[novaDE->top].nota = Notas;
            novaNTD[novaDE->top].faltas = Faltas;
            strcpy(novaNTD[novaDE->top].disciplina,idDisciplina);
            novaNTD[novaDE->top].semestre = auxSem;
            
            novaDE->top++;            
        }

    }
    
    fclose(fp);
}

//Funcao para mostrar todas as disciplinas do usario logado com seu respectivo semestre
void showDisciplinas(int semestre,int *hasDisciplina)
{
    int erro,cred;
    char nomeDisciplina[100];
    char idDisciplina[10];
    *hasDisciplina = 0;

    //RA,CódigodaDisciplina,Semestre,Nota,Faltas
    for(register int i = 0;i<novaDE->top;i++)
    {
        if (user->ra==novaNTD[i].ra && novaNTD[i].semestre==semestre)
        {
            *hasDisciplina = 1;
            erro = consultaDisciplina(novaNTD[i].disciplina,nomeDisciplina,&cred);

            if (erro == 0)
                printf("%s  -   %s  -   Nota: %0.2f    -   Falta: %0.2f\n",novaNTD[i].disciplina,nomeDisciplina,novaNTD[i].nota,novaNTD[i].faltas);  
        }
    }

}


