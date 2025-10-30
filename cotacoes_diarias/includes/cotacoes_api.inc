<?php

/**
 * Busca conteúdo de uma URL com cURL (fallback seguro).
 */
function cotacoes_diarias_fetch_url($url) {
  // Usa cURL direto, com SSL desativado.
  $ch = curl_init($url);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
  curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
  curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10);
  curl_setopt($ch, CURLOPT_TIMEOUT, 15);
  curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36');
  curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);

  $data = curl_exec($ch);
  $error = curl_error($ch);
  $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  curl_close($ch);

  if (!$data || $code != 200) {
    watchdog('cotacoes_diarias', 'Erro ao obter @url: @error', [
      '@url' => $url,
      '@error' => $error,
    ], WATCHDOG_WARNING);
    return (object)['code' => 0, 'data' => ''];
  }

  return (object)['code' => 200, 'data' => $data];
}

/**
 * Retorna as cotações atuais.
 */
function cotacoes_diarias_get_data() {
  $cotacoes = [];

  // === Dólar ===
  $res_dolar = cotacoes_diarias_fetch_url('https://api.exchangerate-api.com/v4/latest/USD');
  $dolar_valor = 0;
  
  if ($res_dolar->code == 200) {
    $data = json_decode($res_dolar->data, true);
    if (isset($data['rates']['BRL'])) {
      $dolar_valor = (float)$data['rates']['BRL'];
      $cotacoes['dolar'] = number_format($dolar_valor, 2, ',', '.');
    }
  }
  
  if (!$dolar_valor) {
    watchdog('cotacoes_diarias', 'Não foi possível obter cotação do Dólar', [], WATCHDOG_WARNING);
  }

  // === Milho ===
  // Usa Yahoo Finance (mercado internacional - Chicago)
  // Milho é cotado em centavos por bushel
  // Conversão: 1 bushel = 25.4 kg, 1 saca 60kg = 2.36 bushels
  $res_milho = cotacoes_diarias_fetch_url('https://query1.finance.yahoo.com/v8/finance/chart/ZC=F');
  
  if ($res_milho->code == 200 && $dolar_valor > 0) {
    $data = json_decode($res_milho->data, true);
    if (isset($data['chart']['result'][0]['meta']['regularMarketPrice'])) {
      $preco_usd_centavos = (float)$data['chart']['result'][0]['meta']['regularMarketPrice'];
      // Converte para R$ por saca de 60kg
      $preco_saca_brl = ($preco_usd_centavos / 100) * 2.36 * $dolar_valor;
      $cotacoes['milho'] = number_format($preco_saca_brl, 2, ',', '.');
    }
  }
  
  if (empty($cotacoes['milho'])) {
    watchdog('cotacoes_diarias', 'Não foi possível obter cotação do Milho', [], WATCHDOG_WARNING);
  }

  // === Soja ===
  // Usa Yahoo Finance (mercado internacional - Chicago)
  // Soja é cotada em centavos por bushel
  // Conversão: 1 bushel = 27.2 kg, 1 saca 60kg = 2.2 bushels
  $res_soja = cotacoes_diarias_fetch_url('https://query1.finance.yahoo.com/v8/finance/chart/ZS=F');
  
  if ($res_soja->code == 200 && $dolar_valor > 0) {
    $data = json_decode($res_soja->data, true);
    if (isset($data['chart']['result'][0]['meta']['regularMarketPrice'])) {
      $preco_usd_centavos = (float)$data['chart']['result'][0]['meta']['regularMarketPrice'];
      // Converte para R$ por saca de 60kg
      $preco_saca_brl = ($preco_usd_centavos / 100) * 2.2 * $dolar_valor;
      $cotacoes['soja'] = number_format($preco_saca_brl, 2, ',', '.');
    }
  }
  
  if (empty($cotacoes['soja'])) {
    watchdog('cotacoes_diarias', 'Não foi possível obter cotação da Soja', [], WATCHDOG_WARNING);
  }

  // Adiciona data de atualização
  $cotacoes['atualizado_em'] = format_date(REQUEST_TIME, 'custom', 'd/m/Y H:i');

  // Se alguma falhar, retorna '--'
  foreach (['dolar', 'milho', 'soja'] as $k) {
    if (empty($cotacoes[$k])) {
      $cotacoes[$k] = '--';
    }
  }

  return $cotacoes;
}
