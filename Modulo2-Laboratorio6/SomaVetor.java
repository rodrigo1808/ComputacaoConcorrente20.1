//Classe da estrutura de dados compartilhado entre as threads
class Vetor {
    
   private int[] numeros = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
   // Construtor inicializando o array com valores de 1 a 10
   public Vetor() { 
      for (int i = 0; i < 10; i++) {
         this.numeros[i] = i+1;
      }
   }

   public synchronized void imprimir() {
      for (int i=0; i<this.numeros.length; i++) {
         System.out.print(this.numeros[i] + " ");
      }
      System.out.println();
   }

   public synchronized int numero(int posicao) {
      return this.numeros[posicao];
   }

   public int tamanho() {
      return this.numeros.length;
   }

   public synchronized void alterarElemento(int posicao, int valor) {
      this.numeros[posicao] = valor;
   }
    
}
  
// Classe que estende Thread e executa a tarefas das threads
class T extends Thread {
   //identificador da thread
   private int id;

   // Numero de threads
   int N;

   // Vetores para as operaçoes
   Vetor A;
   Vetor B;
   Vetor C;
    
   public T(int tid, Vetor A, Vetor B, Vetor C, int numThreads) { 
      this.id = tid; 
      this.N = numThreads;
      this.A = A;
      this.B = B;
      this.C = C;
   }
  
   // Main da thread
   public void run() {
      System.out.println("Thread " + this.id + " iniciou!");
      for (int i = this.id; i < this.C.tamanho(); i += this.N) {
         this.C.alterarElemento(i, (this.A.numero(i) + this.B.numero(i)));
      }
      System.out.println("Thread " + this.id + " terminou!"); 
   }
}
  
// Classe principal da aplicacao
class SomaVetor {
   static final int N = 3;
  
   public static void main (String[] args) {
      //reserva espaço para um vetor de threads
      Thread[] threads = new Thread[N];

      Vetor A = new Vetor();
      Vetor B = new Vetor();
      Vetor C = new Vetor();

      System.out.print("Imprimindo o vetor A: ");
      A.imprimir();

      System.out.print("Imprimindo o vetor B: ");
      B.imprimir();

      System.out.print("Imprimindo o vetor C antes da soma: ");
      C.imprimir();
  
      //cria as threads da aplicacao
      for (int i=0; i<threads.length; i++) {
         threads[i] = new T(i, A, B, C, N);
      }
  
      //inicia as threads
      for (int i=0; i<threads.length; i++) {
         threads[i].start();
      }
  
      //espera pelo termino de todas as threads
      for (int i=0; i<threads.length; i++) {
         try { threads[i].join(); } catch (InterruptedException e) { return; }
      }
  
      System.out.print("Imprimindo o vetor C apos soma: ");
      C.imprimir(); 
   }
}
  