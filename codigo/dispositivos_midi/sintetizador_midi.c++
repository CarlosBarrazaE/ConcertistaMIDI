#include "sintetizador_midi.h++"

void Sintetizador_Midi::iniciar()
{
	m_pid = fork();
	signal(SIGCHLD, SIG_IGN);
	if(m_pid == 0)//En proceso hijo
	{
		char *argumentos[6];
		argumentos[0] = strdup("timidity");
		argumentos[1] = strdup("--interface=A");
		argumentos[2] = strdup("--buffer-fragments=2,8");
		argumentos[3] = strdup("--output-mode=s1l");
		argumentos[4] = strdup("--sampling-freq=44100");
		argumentos[5] = NULL;
		if(execvp(argumentos[0], argumentos) == -1)
		{
			//Error
		}
		_exit(0);
	}
}

void Sintetizador_Midi::esperar_conexion()
{
	bool finalizado = false;
	bool cerrado = false;
	int limite = 0;
	int falta_aqui;
	while(!finalizado && !cerrado && limite < 100)
	{
		//Si hay error termina
		if(waitpid(m_pid, nullptr, WNOHANG) == -1)
			cerrado = true;
		else
		{
			//MidiCommOut::UpdateDeviceList();
			//Timidity crea 4 puertos y probablemente ya existe el puerto falso de linux
			if(true/*MidiCommOut::GetDeviceList().size() > 1*/)
				finalizado = true;
			else
			{
				usleep(10000);
				limite++;
			}
		}
	}
}

void Sintetizador_Midi::detener()
{
	if(m_pid <= 0)
		return;

	for(unsigned int x=0; x<100; x++)
	{
		kill(m_pid, SIGTERM);
		if(waitpid(m_pid, nullptr, WNOHANG))
		{
			m_pid = -1;
			return;
		}
		usleep(10000);
	}

	kill(m_pid, SIGKILL);
	waitpid(m_pid, nullptr, 0);
}
