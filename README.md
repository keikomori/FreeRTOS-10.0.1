# FreeRTOS 10.0.1


Este projeto foi desenvolvido no decorrer das aulas de Sistemas Embarcados com o  objetivo de observar na pratica como é o funcionamento do FreeRTOS em um sistema embarcado, para este trabalho utilizamos o TIVA C.
Foram implementados algumas tarefas para analise do gerenciamento das tarefas pelo FreeRTOS e também implementados alguns drivers.

Para o desenvolvimento do projeto utilizou-se o livro abaixo como consulta:
<a href="https://www.blucher.com.br/livro/detalhes/sistemas-operacionais-de-tempo-real-e-sua-aplicacao-em-sistemas-embarcados-1493">LIVRO: Sistemas Operacionais de Tempo Real e Suas Aplicações em Sistemas Embarcados</a>

### Para quem quer entender um pouco mais como funciona minha sugestão é que se compre o livro, é um super investimento!

# O que é o FreeRTOS?

O FreeRTOS é um kernel de sistema operacional (kernel, ou núcleo, é a parte principal do SO, responsável por todo o sistema) desenvolvido pela Real Time Enginners Ltd. sob a licença MIT. Por ser Open Source, qualquer um pode acessar os códigos fontes do kernel, modificá-los e distribuí-los sob os limites impostos pela licença MIT.

Por ser de código aberto, isso permitiu o surgimento de diversas versões do FreeRTOS suportando uma larga variedade de dispositivos, atualmente existem versões do SO para mais de 35 famílias de processadores.

O FreeRTOS foi projetado para **sistemas embarcados**, sistemas com poucos recursos e pequeno consumo de energia, portanto não implementa muitas das funções executadas por um SO convencional. Geralmente é encarado como uma biblioteca de multi-thread (ou multitasking) ao invés de um sistema operacional propriamente dito, entretanto algumas funções definidas pela POSIX (POSIX – Portable Operational System Interface, ou Interface Portável entre Sistemas Operacionais),  um conjunto de normas para sistemas operacionais, seguida pelo Unix e Linux, estão implementadas no kernel FreeRTOS.

O FreeRTOS é um SO preemptivo, ou seja, permite a execução de tarefas em paralelo (multitasking) de forma que cada tarefa acesse o processador por um certo período de tempo (quantum) e, ao término, troque automaticamente para a próxima tarefa. Nesse processo todos os dados da tarefa corrente são salvos de forma que, ao obter novamente direito ao processador, a tarefa continue no exato ponto onde parou, como se nunca tivesse parado.

Os critérios para escolha da próxima tarefa variam, geralmente define-se uma prioridade para cada tarefa. Dependendo da forma que o sistema escolhe a próxima tarefa pode ser necessário um cuidado adicional para evitar que uma tarefa nunca ganhe direito ao processador, pois sempre há uma tarefa mais prioritária que ela, essa situação é chamada de inanição (ou starvation). SOs mais complexos implementam sistema de prioridade dinâmica, aumentando a prioridade das tarefas não escolhidas a fim de evitar a inanição.

O gerenciamento de tarefas pode ser feito de formas diferentes em outros sistemas operacionais. Além dos sistemas preemptivos destacam-se também os de lote (batch) que executam apenas uma única tarefa por vez de forma sequencial (não é muito utilizado atualmente, alguns sistemas mais antigos, como o MS-DOS da Microsoft, utilizavam este gerenciamento) e os cooperativos, que não realizam a troca automática de tarefas e exigem que a tarefa corrente termine ou abra mão do processador para executar outra.

A imagem abaixo ilustra o uso do processador, por diferentes Tarefas (T1, T2 e T3), nestas três formas de gerenciamento de tarefas.

<img src=”https://uploads.filipeflop.com/2019/10/Imagem3-1024x683.png”>

No primeiro caso, cada tarefa termina antes de iniciar a próxima. No segundo, a tarefa corrente pode abrir mão do processador antes de terminar de executar, nesse caso, a próxima tarefa assume. E no terceiro caso, o SO altera a tarefa corrente a cada período (quantum).

Repare que apesar de ser multitasking, as tarefas não rodam, de fato, em paralelo, mas uma de cada vez de forma alternada. Isso pois estamos considerando um processador singlecore, no caso de um multicore é possível o verdadeiro paralelismo, cada tarefa rodando em um core diferente.

Como descrito pelo nome, o FreeRTOS é um RTOS (Real-time Operational System), ou Sistema Operacional de Tempo Real, cuja resposta temporal deve ser conhecida no melhor e no pior caso. Deve ser possível estabelecer prazos mínimos e máximos para conclusão de uma tarefa que devem ser respeitados, caso contrário pode-se configurar uma falha grave. Geralmente este tipo de SO prioriza tempo ao invés de quantidade, ou seja, suporta menos tarefas paralelas mas que possuem comportamento temporal bem definido.

O FreeRTOS permite a alocação dinâmica ou estática de memória para as tarefas e ferramentas do sistema.

No caso da alocação dinâmica, basta utilizar as funções para criar as tarefas, o próprio sistema irá reservar a memória necessária para a execução da mesma de forma dinâmica. Este método é mais prático e permite a criação dinâmica de tarefas, mas os erros relacionados à falta de memória não são detectados em tempo de compilação, ou seja, o desenvolvedor só saberá se ultrapassou o limite de memória do sistema ao rodar a aplicação e perceber alguma inconsistência.

No caso da alocação estática, necessita-se reservar manualmente as áreas de memória necessárias para as tarefas antes de criá-las, isso é feito na forma de declaração de variáveis. Esse método é menos prático, mas ajuda a lidar com situações de memória limitada, os erros referentes a falta de memória são descobertos em tempo de compilação.

Existe um arquivo de configuração do sistema operacional denominado FreeRTOSConfig.h, no qual é possível customizar o comportamento do sistema através da declaração de macros, é necessário incluir uma cópia deste arquivo no diretório local do projeto, a descrição deste arquivo e das macros utilizadas pode ser encontrado no site do FreeRTOS.

Para mais informações:

<a href="https://github.com/freertos/freertos">FreeRTOS</a>

