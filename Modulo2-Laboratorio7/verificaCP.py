class Monitor:
	def __init__(self):
		self.numElementos = 0

	def consumidorBloqueado(self,id):
	    '''Recebe o id do consumidor. Verifica se a decisao de bloqueio esta correta.'''
	    if(self.numElementos != 0):
		    print("ERRO: consumidor " + str(id) + " bloqueado quando existe elementos para consumir!")

	def produtorBloqueado(self,id):
	    '''Recebe o id do produtor. Verifica se a decisao de bloqueio esta correta.'''
	    if(self.numElementos < 10):
		    print("ERRO: produtor " + str(id) + " bloqueado quando ha espaco para inserir elemento!")

	def consumidorRetirandoElemento(self,id):
		'''Recebe o id do consumidor, verifica se pode remover algum elemento e informa que foi removido.'''
		if(self.numElementos == 0):
			print("ERRO: consumidor " + str(id) + " esta retirando elemento quando nao existe nenhum elemento!")
		self.numElementos-=1

	def produtorInserindoElemento(self,id):
		'''Recebe o id do produtor, verifica se pode inserir um elemento e informa que tem um elemento a mais'''
		if(self.numElementos > 9):
			print("ERRO: produtor " + str(id) + " esta inserindo elemento quando nao existe espaco!")
		self.numElementos+=1

	def produtorSaindo(self,id):
		'''Recebe o id do produtor que terminou a inserção.'''

	def consumidorSaindo(self,id):
		'''Recebe o id do consumidor que terminou a remoção.'''