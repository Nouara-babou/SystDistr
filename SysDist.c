#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int main(){
	int nb_processus;
	int identifiant;
	int pere;
	int *fils;
	int taille=0;
	int fin = 0;
	int c,i;
	MPI_Status status;
	int origine_id;
	int balise;
	int taille_msg;
	char buff[10]={"      "};
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&nb_processus);
	MPI_Comm_rank(MPI_COMM_WORLD,&identifiant);
	// le tableau pour stocker les fils 
	fils = malloc(sizeof(int)*nb_processus);
	if(identifiant == 0){
		// 2deg(0)
		
	/* Au debut la racine envoie a tout le monde  un message de type donné mais
 	ca se peut que un message prend plus  temps pour arriver à l'une des entités des calculs que le message envoyé 
	par l entité de calcul qui a recu vite le message prevenant de la racine */

		for(i=1;i<nb_processus;i++){
					
			// un message de type donnée est envoyé sur balise 0
			MPI_Send("11609276",8,MPI_CHAR,i,0,MPI_COMM_WORLD);
			printf("le message a ete envoye pour mon voisin %d\n",i);
			c=2*(nb_processus-1);	
		}

	}
	else {// les autres unités de calcul recevront les messages le premier message (type donnée) determinera le pére de cette entité 
		// pour connaitre le pere 
		MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
		origine_id = status.MPI_SOURCE;
		pere = origine_id;
		// On recoit un message de type donnée donc sur la balise 0 

		// Tant que l'unité de calcul n a pas recu un message de type donnée elle sera bloquée 
		MPI_Recv(buff,8,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
		// A  son tour chaque unité de calcul enverra des messages de type donnée aux autres unités de calculs differentes de elle 				meme et son pére 
		for(i=0;i<nb_processus;i++){ 
			if(i != origine_id && i != identifiant){
				MPI_Send("11609276",8,MPI_CHAR,i,0,MPI_COMM_WORLD);
				c=2*(nb_processus-1)-2;
				
				
			}
		}
	}

	while(c >= 0){

		MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
		origine_id = status.MPI_SOURCE;
		MPI_Get_count(&status,MPI_INT, &taille_msg);	
		balise=status.MPI_TAG;
		MPI_Recv(buff,taille_msg,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		

		
		if(balise == 0){
//si le message est de type donnée celui ci n est pas considéré 
//comme pere car le pere est deja dans le else donc j envoie juste un accusé de réception pour dire que j 'ai recu ou tu n'est pas mon pére 

			c--;
			MPI_Send("accuse",6,MPI_CHAR,origine_id,2,MPI_COMM_WORLD);
		}// si msg est  de type pére donc on informe l'unité de calcul de la liste de ses enfants on ajoutons son enfant 
		else if(balise == 1){
			// on l ajoute a la derniere case du tableau 
			fils[taille] = origine_id;
			taille++;
			c=c-2;
		}// si de type accusé on fait rien 		
		else if(balise == 2){
			c--;
		}
	}// la racine n est pas pere 
	if(identifiant != 0){
			
	/*chaque unité de calcul enverra un message de type pére a son pere pour qu 'elle soit informée de la liste des fils*/ 			  MPI_Send("pere",4,MPI_CHAR,pere,1,MPI_COMM_WORLD);
		MPI_Recv(buff,1,MPI_INT,pere,3,MPI_COMM_WORLD,&status);
	}
	fin = 1;
	for(i=0;i<taille;i++){
		// le pere va envoyer un message de type fin a son enfant

		MPI_Send(&fin,1,MPI_INT,fils[i],3,MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}

