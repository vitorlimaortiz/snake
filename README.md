---

## 1 ESTRUTURAS DE DADOS

### 1.1 Lista Encadeada circular

A cobra foi implementada por meio de uma lista encadeada circular simples:

* Cada nó representa um segmento da cobra
* gestão dinâmica: utlizando malloc e free
* Vantagem na movimentação

**Vantagens:**

* crescimento dinamico infinito 
* Flexibilidade no crescimento
* uso de memoria sob demanda

**Desvantagem:**

* Remoção da cauda

### 1.2 Vetor de Frutas

As frutas são gerenciadas por um vetor estatico de structs (comida)

* Acesso direto
* Quantidade (Total_frutas)

---

## 2. ORGANIZAÇÃO DO SISTEMA

O sistema foi dividido em módulos funcionais:

### 2.1 Menu

* Exibição do menu principal
* Seleção de dificuldade
* Cálculo de Offset Dinâmico

### 2.2 Loop Principal

O jogo opera em um loop contínuo contendo:

* Captura de entrada do usuário (ler teclado)
* Atualização logica (processarLogica)
* Renderização
* Controle de fremerate (sleep)

### 2.3 Atualização do Jogo

Responsável por:

* Movimentação da cobra (deltaX, deltaY)
* Verificação de colisões (detectarImpacto)
* Consumo de frutas e crescimento (lista_comidas)
* Gestão de Pontuação e Recorde: Cada fruta consumida incrementa a variável pontos em 15 unidades.

### 2.4 Sistema de Níveis

Sistema de Dificuldade e Velocidade (configurarModo) (delay)

* Ajuste de velocidade

### 2.5 Tela de Game Over

Permite ao usuário:

* Reiniciar o jogo
* Retornar ao menu
* Encerrar o jogo

---

## 3. ANÁLISE

### 3.1 Variáveis

LARGURA_MAPA (W) e ALTURA_MAPA (H)LARGURA_MAPA (W) e ALTURA_MAPA (H): Definem a área útil de jogo (80x25).

TOTAL_FRUTAS (F): Define a quantidade fixa de alimentos gerados simultaneamente (10 unidades).

pontos (S): Representa o progresso do jogador (o tamanho da cobra é derivado do número de nós na lista encadeada, crescendo conforme os pontos aumentam).

din_offset_x e din_margem_superior: Variáveis de ajuste para centralizar o desenho na tela do console.


### 3.2 Principais Funções

####  processamento logico

#### Verificação de Colisão

#### Geração de Frutas

#### Renderização dinamica 

#### Atualização de Nível
---

## 4. PONTOS CRÍTICOS

### 4.1 Remoção da Cauda

### 4.2 sicronização do teclado

### 4.3 Geração de Frutas

---

## 5. POSSÍVEIS MELHORIAS

* Obstáculos e Mapas
* Adição de "Power-ups"
* melhorias na interface e desing (UX/UI)
* Dificuldade Progressiva

---

## 8. FUNCIONALIDADES DO SISTEMA

*O sistema desenvolvido é capaz de:
* Movimentação Fluida 
*Níveis de Dificuldade
*Sistema de Recorde
*Interface Dinâmica
*Sistema Multi-Frutas
*Gestão de Estado: Menus interativos, pausa e tela de Game Over com opções de retorno.

---
