/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variante.h"
#include "readcmd.h"
#include <fcntl.h>

#include <unistd.h>
#include <sys/wait.h>

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */


#include <libguile.h>



int question6_executer(char *line)
{
	/* Question 6: Insert your code to execute the command line
	 * identically to the standard execution scheme:
	 * parsecmd, then fork+execvp, for a single command.
	 * pipe and i/o redirection are not required.
	 */
     
			/* parsecmd free line and set it up to 0 */
		struct cmdline *l;
		l = parsecmd( & line);
		int pid = fork();
		if(pid == 0){
			execvp(l->seq[0][0],l->seq[0]);
		}
		else{
				wait(NULL); 
			}

		/* Remove this line when using parsecmd as it will free it */
	free(line);
	
	return 0;
}

SCM executer_wrapper(SCM x)
{
        
		return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}



void terminate(char *line) {
#if USE_GNU_READLINE == 1
	/* rl_clear_history() does not exist yet in centOS 6 */
	clear_history();
#endif
	if (line)
	  free(line);
	printf("exit\n");
	exit(0);
}


int main() {
        printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
        scm_init_guile();
        /* register "executer" function in scheme */
        scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif
    
	struct processes_bg* bg_list = NULL; 

	while (1) {
		char *prompt = "ensishell>";
		struct cmdline *l;
		char *line=0;
		int i, j;
		

		/* Readline use some internal memory structure that
		   can not be cleaned at the end of the program. Thus
		   one memory leak per command seems unavoidable yet */
		line = readline(prompt);
		if (line == 0 || ! strncmp(line,"exit", 4)) {
			terminate(line);
		}

#if USE_GNU_READLINE == 1
		add_history(line); 
#endif


#if USE_GUILE == 1
		/* The line is a scheme command */
		if (line[0] == '(') {
			char catchligne[strlen(line) + 256];
			sprintf(catchligne, "(catch #t (lambda () %s) (lambda (key . parameters) (display \"mauvaise expression/bug en scheme\n\")))", line);
			scm_eval_string(scm_from_locale_string(catchligne));
			free(line);
                        continue;
                }
#endif

		/* parsecmd free line and set it up to 0 */
		l = parsecmd( & line);
		

		/* If input stream closed, normal termination */
		if (!l) {
		  
			terminate(0);
		}
		

		
		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);
		if (l->bg) printf("background (&)\n");
        
		/* Display each command of the pipe */

		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
                        for (j=0; cmd[j]!=0; j++) {
                                printf("'%s' ", cmd[j]);
                        }
			printf("\n");
		}

        


        // DEBUT DE NOTRE CODE ON A CHOISI POUR VARIANCE LE PIPE MULTIPLE 



        // CONSEVER STDOUT ET STDIN POUR TRAITER LE CAS DES REDIRECTIONS

        int saved_stdout = dup(1);
        int saved_stdin = dup(0);

	    if( l->seq[0]!=NULL){	

			// CAS DES COMMMANDES AVEC DES REDIRECTIONS 
			if(l->in){
				int infile=open(l->in,O_RDONLY|O_CREAT,0777);
				dup2(infile,0);
				close(infile);
			}
			if(l->out){
				int outfile=open(l->out,O_WRONLY|O_CREAT,0777);
				ftruncate(outfile, 0);
				dup2(outfile,1);
				
				close(outfile);							
			}
			// PIPE NORMAL 

			if (l->seq[1]!=0 && l->seq[2]==0 ){
				int pipeProcess = fork();
				if (pipeProcess == 0){
					int tuyau[2];
					pipe(tuyau);
					int res;
					if((res=fork())==0) {
						dup2(tuyau[0], 0);
						close(tuyau[1]); close(tuyau[0]);
						execvp(l->seq[1][0],l->seq[1]);
					}
					dup2(tuyau[1], 1);
					close(tuyau[0]); close(tuyau[1]);
					execvp(l->seq[0][0],l->seq[0]);
				}
				else{
					wait(NULL);
				} 
			}
        // PIPE MULTIPLE 
        if (l->seq[1]!=0 && l->seq[2]!=0){
			    int tuyau[2];
				int pipe_in= 0;
				int i =0 ;				
                while (l->seq[i]!=0) {
					pipe(tuyau);
					int pid= fork();				
					if (pid == 0) {
						dup2(pipe_in, 0);
						if (l->seq[i+1]!=0) {
							dup2(tuyau[1], 1);
							}
						close(tuyau[0]);
						execvp(l->seq[i][0], l->seq[i]);
					}		
					else {
						wait(NULL); 		
						close(tuyau[1]);
						pipe_in = tuyau[0];
						i++;
					}
				} 

			}
			// 	INTERPRETEUR DES COMMANDES SIMAPLES ET JOBS 

			if( !strcmp(l->seq[0][0],"cd")){
				// CAS DE CD 
				if (chdir(l->seq[0][1]) != 0) {
					printf("error when changing directory");
					return 0 ;
				}
			}
			if (!strcmp(l->seq[0][0],"jobs")){
				// CAS DE JOBS 
				if(bg_list != NULL){
					int status;
					struct processes_bg* parcours_listbg=bg_list;
					int waitingId = waitpid(parcours_listbg->val,&status,WNOHANG);
						if(waitingId == 0)  // child still running
							printf("%d is running on backgroud  %s \n",parcours_listbg->val,parcours_listbg->cmd);
					while (parcours_listbg->next != NULL){
						parcours_listbg=parcours_listbg->next;
						int waitingId = waitpid(parcours_listbg->val,&status,WNOHANG);
						if(waitingId == 0){  // child still running
							printf("%d is running on backgroud  %s \n",parcours_listbg->val,parcours_listbg->cmd);
						}
					}
				}

			}
			else{
				// CAS DU RESTE DES COMMANDES SIMPLES 
				int pid = fork();
				if(pid == 0){
					execvp(l->seq[0][0],l->seq[0]);
				}
				else{
					if(l->bg == 0){
						wait(NULL); 
					}else{
						printf("%d\n",pid);
						// CONSERVER LES COMMANDES LANCÉES EN TACHE DE  AU FOND DANS UNE LISTE CHAINÉÉ POUR LA COMMANDE JOBS 
						struct processes_bg* new =(struct processes_bg*)malloc(sizeof(struct processes_bg));
						new->val=pid;
						new->next= bg_list;
						new->cmd=(char*)malloc(sizeof(char*));
						strcpy(new->cmd,l->seq[0][0]);
						bg_list = new;					}
					
					}
			}

		}
		// REMISE DES SORTIES ET ENTÉES STANDART APRES UNE REDIRECTION POTENTIELLE
		dup2(saved_stdout, 1);
        close(saved_stdout);
		dup2(saved_stdin, 0);
        close(saved_stdin);
	}
    // FREE LA LISTE CHAINÉE CRÉÉE POUR JOBS 
	struct processes_bg* parcours_listbg=bg_list;
	if(parcours_listbg!=NULL){
		while (parcours_listbg->next != NULL){
			free(parcours_listbg->cmd);
			free(parcours_listbg);
			parcours_listbg=parcours_listbg->next;
		}
	}


}
