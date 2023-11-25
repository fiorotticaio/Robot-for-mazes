# Robô para labirinto

<img src="board.jpeg">

## Convenções
- **Código** deve ser escrito em **português**
- **Variáveis** devem ser escritas em **português**

## Variáveis
- Nome das **variáveis** devem ser escritas em **snake_case**

## Funções
- Nome das **funções** devem ser escritas em **snake_case**

## Arquivos
- Nome dos **arquivos** devem ser escritos em **snake_case**

## Implementações
- [ ] Segue a linha (Controle proporcional)
    - Dois sensores infrevermelho da frente, dependendo do valor da leitura de cada um, ativa mais ou menos os motores DC das 
      rodas da direita e esquerda
- [ ] Realizar cruzamentos (90 direita, esquerda e 180 graus) 
    - Ao detectar um cruzamento (os outros 3 sensores infea-vermelho lerem preto), deve-se girar 90 graus para a esquerda
      e ir vendo se não tem parede na frente com o ultrassom, se não tiver parade na frente segue reto, se tiver vira de novo
      90 graus para a esquerda ate poder ir reto
- [x] Ler distância de obstáculos com o ultrasom e realizar filtragem - Sugestão, média móvel! 
- [x] Visualizar parâmetros do Robô no LCD (Distância do Ultrassom, sensores de linha, e estado da bateria) 
- [ ] Visualizar todos os sensores do Robô em dasboard Grafana ou similar
- [ ] Sair do labirinto

## Bugs

## Melhorias