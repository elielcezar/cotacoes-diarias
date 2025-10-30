# Cotações Diárias - Módulo Drupal 7

[![Drupal](https://img.shields.io/badge/Drupal-7.x-blue.svg)](https://www.drupal.org)
[![License](https://img.shields.io/badge/license-GPL--2.0-green.svg)](LICENSE)
[![PHP](https://img.shields.io/badge/PHP-7.4%2B-purple.svg)](https://php.net)

Módulo para Drupal 7 que exibe cotações diárias de **Dólar**, **Milho** e **Soja** em tempo real no seu site.

![Cotações Diárias](https://img.shields.io/badge/Status-Funcionando-success)

---

## 📋 Descrição

Este módulo cria um bloco personalizável que exibe as cotações atualizadas de:

- 💵 **Dólar** (USD/BRL)
- 🌽 **Milho** (R$/saca de 60kg)
- 🌱 **Soja** (R$/saca de 60kg)

As cotações são buscadas automaticamente de APIs públicas e atualizadas a cada hora, com sistema de cache para otimizar performance.

---

## ✨ Funcionalidades

- ✅ Cotações em tempo real via APIs públicas
- ✅ Sistema de cache inteligente (1 hora)
- ✅ Fallback automático para "--" em caso de erro
- ✅ Logging de erros via Watchdog
- ✅ Bloco totalmente customizável
- ✅ Sem necessidade de chaves de API
- ✅ Leve e otimizado

---

## 🔧 Requisitos

- **Drupal**: 7.x
- **PHP**: 7.4 ou superior
- **Extensões PHP**:
  - `curl` (para requisições HTTP)
  - `json` (para parsing de dados)
- **Conexão externa**: Servidor precisa acessar APIs externas via HTTPS

---

## 📦 Instalação

### 1. Download do Módulo

```bash
cd sites/all/modules/custom/
git clone https://github.com/seu-usuario/cotacoes_diarias.git
```

### 2. Ativar o Módulo

Via Drush:
```bash
drush en cotacoes_diarias -y
drush cc all
```

Ou via interface administrativa:
1. Acesse `Admin > Modules`
2. Localize "Cotações Diárias"
3. Marque a checkbox e clique em "Save configuration"

### 3. Adicionar o Bloco

1. Acesse `Admin > Structure > Blocks`
2. Localize o bloco "Cotações do dia"
3. Selecione a região desejada (ex: Sidebar, Header, Footer)
4. Clique em "Save blocks"

---

## 🎯 Como Usar

Após a instalação, o bloco exibirá automaticamente:

```
Cotações do dia
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
💵 Dólar: R$ 5,35 | 🌽 Milho: R$ 54,86/sc | 🌱 Soja: R$ 130,59/sc
Atualizado: 30/10/2025 10:25
* Milho e Soja: valores aproximados do mercado internacional
```

### Limpar Cache Manualmente

Se precisar forçar atualização das cotações:

```bash
drush cc all
```

Ou via código:
```php
cache_clear_all('cotacoes_diarias_cache', 'cache');
```

---

## 🌐 APIs Utilizadas

### 1. **Dólar (USD/BRL)**
- **API**: [ExchangeRate-API](https://www.exchangerate-api.com/)
- **Endpoint**: `https://api.exchangerate-api.com/v4/latest/USD`
- **Tipo**: Gratuita, sem necessidade de chave
- **Atualização**: Diária

### 2. **Milho (Corn Futures)**
- **API**: Yahoo Finance
- **Símbolo**: `ZC=F` (Chicago Board of Trade)
- **Endpoint**: `https://query1.finance.yahoo.com/v8/finance/chart/ZC=F`
- **Conversão**: Centavos/bushel → R$/saca de 60kg
- **Nota**: Valores do mercado internacional (Chicago)

### 3. **Soja (Soybean Futures)**
- **API**: Yahoo Finance
- **Símbolo**: `ZS=F` (Chicago Board of Trade)
- **Endpoint**: `https://query1.finance.yahoo.com/v8/finance/chart/ZS=F`
- **Conversão**: Centavos/bushel → R$/saca de 60kg
- **Nota**: Valores do mercado internacional (Chicago)

---

## 🔄 Conversões Matemáticas

### Milho
```
Preço em Chicago (USD/bushel) → R$/saca de 60kg
Conversão: (preço_usd_centavos / 100) × 2.36 × taxa_dolar
Onde: 1 bushel = 25.4 kg, 1 saca = 60kg = 2.36 bushels
```

### Soja
```
Preço em Chicago (USD/bushel) → R$/saca de 60kg
Conversão: (preço_usd_centavos / 100) × 2.2 × taxa_dolar
Onde: 1 bushel = 27.2 kg, 1 saca = 60kg = 2.2 bushels
```

---

## 📁 Estrutura do Módulo

```
cotacoes_diarias/
│
├── cotacoes_diarias.info          # Informações do módulo
├── cotacoes_diarias.module         # Hook implementations
├── includes/
│   └── cotacoes_api.inc           # Funções de API
└── README.md                       # Este arquivo
```

---

## 🐛 Troubleshooting

### Problema: Cotações exibem "--"

**Causas possíveis:**

1. **Firewall bloqueando requisições externas**
   ```bash
   # Teste manualmente
   curl https://api.exchangerate-api.com/v4/latest/USD
   ```

2. **cURL não instalado**
   ```bash
   php -m | grep curl
   # Se não aparecer, instale:
   sudo apt-get install php-curl
   ```

3. **Cache antigo**
   ```bash
   drush cc all
   ```

4. **Verificar logs**
   - Acesse: `Admin > Reports > Recent log messages`
   - Filtre por: `cotacoes_diarias`

### Problema: Valores parecem incorretos

- **Milho e Soja**: São valores do mercado de Chicago (EUA), convertidos para R$
- Podem diferir do CEPEA (mercado brasileiro)
- São apenas valores de referência aproximados

### Problema: Módulo não aparece

```bash
drush cc all
drush en cotacoes_diarias -y
```

---

## 🎨 Customização

### Alterar Tempo de Cache

Edite `cotacoes_diarias.module`, linha ~33:

```php
// Altere 3600 (1 hora) para o valor desejado em segundos
if ($cache && !empty($cache->data) && isset($cache->data['atualizado_em']) && (REQUEST_TIME - $cache->created < 3600)) {
```

### Estilizar o Bloco

Adicione CSS customizado no seu tema:

```css
.cotacoes-diarias {
  background: #f0f0f0;
  padding: 15px;
  border-radius: 5px;
  font-size: 14px;
}

.cotacoes-diarias strong {
  color: #2c5282;
}
```

### Adicionar Mais Commodities

Edite `includes/cotacoes_api.inc` e adicione novas fontes seguindo o padrão existente.

**Exemplos de símbolos Yahoo Finance:**
- `GC=F` - Ouro
- `CL=F` - Petróleo
- `HG=F` - Cobre
- `NG=F` - Gás Natural

---

## 🔒 Segurança

- ✅ Todos os valores são sanitizados com `check_plain()` antes de exibição
- ✅ SSL verificação desabilitada apenas para APIs públicas confiáveis
- ✅ Timeout de conexão configurado (10s)
- ✅ Sem armazenamento de dados sensíveis

---

## 📊 Performance

- **Cache**: 1 hora (reduz requisições às APIs)
- **Timeout**: 10 segundos de conexão, 15 segundos total
- **Impacto**: Mínimo - requisições apenas quando cache expira
- **Sem JS**: Renderização server-side, mais rápido

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Por favor:

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/MinhaFeature`)
3. Commit suas mudanças (`git commit -m 'Adiciona MinhaFeature'`)
4. Push para a branch (`git push origin feature/MinhaFeature`)
5. Abra um Pull Request

---

## 📝 Changelog

### v1.0.0 (2025-10-30)
- ✅ Versão inicial
- ✅ Cotação do Dólar (ExchangeRate-API)
- ✅ Cotação do Milho (Yahoo Finance)
- ✅ Cotação da Soja (Yahoo Finance)
- ✅ Sistema de cache
- ✅ Logging de erros

---

## 📄 Licença

Este projeto está licenciado sob a [GNU General Public License v2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html) - veja o arquivo LICENSE para detalhes.

---

## 🙏 Agradecimentos

- [ExchangeRate-API](https://www.exchangerate-api.com/) - API gratuita de câmbio
- [Yahoo Finance](https://finance.yahoo.com/) - Dados de commodities
- Comunidade Drupal Brasil

---

## ⚠️ Disclaimer

Este módulo utiliza APIs públicas e gratuitas. Os valores exibidos são apenas para referência e não devem ser usados para decisões financeiras. Milho e Soja refletem o mercado internacional (Chicago) e podem diferir significativamente dos preços brasileiros (CEPEA).

---

**Se este módulo foi útil, deixe uma ⭐ no GitHub!**
