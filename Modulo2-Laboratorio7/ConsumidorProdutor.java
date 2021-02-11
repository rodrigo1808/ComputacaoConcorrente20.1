/* 
    A classe Buffer tem sua estrutura semelhante a de uma pilha.
    Um elemento só é inserido no topo da lista e só pode remover o elemento do topo da lista,
    dessa forma satisfazendo a condição do problema de só poder inserir quando o buffer não estiver cheio
    e remover só quando tiver algum elemento.
*/
class Buffer {
    private int[] vetor = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    public int indexVazio = 0;
    
    // Construtor
    Buffer() { 
        for (int i = 0; i < 10; i++) {
            this.vetor[i] = -1; // O valor -1 significa "vazio" para as posições
        }
    }

    // Método que imprime os valores buffer
    public synchronized void ImprimeBuffer () {
        for (int i = 0; i < 10; i++) {
            System.out.print(this.vetor[i] + " ");
        }
        System.out.println("");
    }


    // Método que insere um elemento, apenas se existir espaço
    public synchronized void InsereElemento(int valor) {
        if(indexVazio < 9) {
            vetor[indexVazio] = valor;
            indexVazio++;
        }
    }

    // Método que retira um elemento apenas se possuir algum elemento
    public synchronized void RetiraElemento() {
        if(indexVazio > 0) {
            vetor[indexVazio] = -1;
            indexVazio--;
        }
    }
}

// Monitor
class Monitor {
    private int numElementos;
    
    // Construtor
    Monitor(Buffer b) { 
        this.numElementos = 0;
    }
    
    // Entrada para Consumidores
    public synchronized void EntraConsumidor (int id) {
        try { 
            while(this.numElementos < 1) {
                System.out.println ("Monitor.consumidorBloqueado("+id+")");
                wait();
            }
            this.numElementos--;
            System.out.println ("Monitor.consumidorRetirandoElemento("+id+")");
        } catch (InterruptedException e) { }
    }
    
    // Saida para Consumidores
    public synchronized void SaiConsumidor (int id) {
        if(this.numElementos < 10)
            notify(); //libera Produtor caso tenha espaço para inserir
        System.out.println ("Monitor.consumidorSaindo("+id+")");
    }
    
    // Entrada para Produtores
    public synchronized void EntraProdutor (int id) {
      try { 
        while(this.numElementos == 10) {
           System.out.println ("Monitor.produtorBloqueado("+id+")");
           wait();
        }
        this.numElementos++;
        System.out.println ("Monitor.produtorInserindoElemento("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para Produtores
    public synchronized void SaiProdutor (int id) {
        notify();
        System.out.println ("Monitor.produtorSaindo("+id+")");
    }
}

class Produtor extends Thread {
    int id; // Id da thread
    Buffer buffer; // Buffer compartilhado
    Monitor monitor; // Monitor para coordenar a lógica de execução das threads
  
    // Construtor
    Produtor (int id, Buffer b, Monitor m) {
        this.id = id;
        this.buffer = b;
        this.monitor = m;
    }
  
    // Método executado pela thread
    public void run () {
        for (;;) {
            this.monitor.EntraProdutor(this.id); 
            buffer.InsereElemento(this.id);
            this.monitor.SaiProdutor(this.id); 
        }
    }
}

class Consumidor extends Thread {
    int id; // Id da thread
    Buffer buffer;
    Monitor monitor; // Monitor para coordenar a lógica de execução das threads
  
    // Construtor
    Consumidor (int id, Buffer b, Monitor m) {
        this.id = id;
        this.buffer = b;
        this.monitor = m;
    }
  
    // Método executado pela thread
    public void run () {
        for (;;) {
            this.monitor.EntraConsumidor(this.id); 
            buffer.RetiraElemento();                                                                                                                                                                                                                                                                                    
            this.monitor.SaiConsumidor(this.id); 
        }
    }
}

public class ConsumidorProdutor {
    static final int P = 4;
    static final int C = 3;
    public static void main (String[] args) {
        int i;
        Buffer buffer = new Buffer(); // Criação do buffer que será compartilhado entre as Threads
        Monitor monitor = new Monitor(buffer); // Criação do monitor para controlar a concorrência e o buffer compartilhado
        Consumidor[] c = new Consumidor[C]; // Threads Consumidoras
        Produtor[] p = new Produtor[P]; // Threads Produtoras

        //inicia o log de saida
        System.out.println ("import verificaCP");
        System.out.println ("Monitor = verificaCP.Monitor()");

        for (i=0; i<P; i++) {
            p[i] = new Produtor(i+1, buffer, monitor);
            p[i].start(); 
        }
        for (i=0; i<C; i++) {
            c[i] = new Consumidor(i+1, buffer, monitor);
            c[i].start(); 
        }
    }
}
