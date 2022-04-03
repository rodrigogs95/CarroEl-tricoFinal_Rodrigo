Para a ambulância em questão foram feitas 3 aplicações que são de suma importância para o tráfego do veículo no trânsito:

1 - Luzes: Foi utilizado um circuito com 2 Mosfetes canais P e 2 canais N, além de um inversor. Seu funcionamento baseia-se em:

	- No Mosfet canal N, quanto mais positivo a tensão entre o gate e source, maior será a corrente no dreno, o que aumenta a corrente na carga.

	- No canal P, quanto mais negativa for a tensão VGS maior a corrente no dreno.

    Assim, temos uma variação na saída do Atmega que faz com que hora se tenha 5V e hora 0V, fazendo com que cada conjunto de mosfets conduzam por vez, assim tem-se um sistema de cores Vermelho, Branco e Azul piscando em sincronia.


2 - Som: Para maior mobilidade no trânsito o som da ambulância é de fundamental importância. Foi colocado uma saída de áudio em que essa é ativada pelo monitor serial:
	- s: Inverte o estado do circuito (Se estiver desligado, liga).


3 - Farol: Foi implementado um Farol que é acionado pelo monitor serial e seu funcionamento é dado por:
	- f: Liga o circuito;
	- n: Desliga o circuito;


Dica: Se enviar 'sf' no monitor serial liga o farol e o som e pode-se fazer as demais combinações.

