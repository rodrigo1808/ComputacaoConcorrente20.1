class Monitor:
	def __init__(self):
		self.numElementos = 0

	def consumidorBloqueado(self,id):
	    '''Recebe o id do leitor. Verifica se a decisao de bloqueio esta correta.'''
	    if(self.numElementos != 0):
		    print("ERRO: consumidor " + str(id) + " bloqueado quando existe elementos para consumir!")

	def produtorBloqueado(self,id):
	    '''Recebe o id do escritor. Verifica se a decisao de bloqueio esta correta.'''
	    if(self.numElementos < 10):
		    print("ERRO: produtor " + str(id) + " bloqueado quando ha espaco para inserir elemento!")

	def consumidorRetirandoElemento(self,id):
		'''Recebe o id do leitor, verifica se pode ler e registra que esta lendo.'''
		if(self.numElementos == 0):
			print("ERRO: consumidor " + str(id) + " esta retirando elemento quando nao existe nenhum elemento!")
		self.numElementos-=1

	def produtorInserindoElemento(self,id):
		'''Recebe o id do escritor, verifica se pode escrever e registra que esta escrevendo'''
		if(self.numElementos > 9):
			print("ERRO: produtor " + str(id) + " esta inserindo elemento quando nao existe espaco!")
		self.numElementos+=1

	def produtorSaindo(self,id):
		'''Recebe o id do produtor que terminou a inserção.'''

	def consumidorSaindo(self,id):
		'''Recebe o id do consumidor que terminou a remoção.'''