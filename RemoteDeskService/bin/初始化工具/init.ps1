param([int]$pindex,[String]$configFilePath,[String]$url)
function Set-RegistryValue([String]$key, [String]$item, [String]$value, $type="String") 
{ 
  if ((Test-Path $key) -eq $false)
  {
  	md $key | Out-Null 
  } 
  Set-ItemProperty -Path  $key -Name $item -Value $value;
}

$items = Get-WmiObject -class Win32_NetworkAdapter  -namespace "root\cimv2" | where {$_.MACAddress -ne $null}
$mac = $null;
foreach($item in $items)
{
	if($item.Index -eq $pindex)
	{
		$mac = $item.MACAddress;
	}
}
if($mac -eq $null)
{
	Write-Host "没有倒找指定的MAC地址";	
	exit;
}

$url = $url + $mac;
$webClient = New-Object -TypeName System.Net.WebClient;
try
{
	$byteContents = $webClient.DownloadData($url);
}
catch
{
	Write-Host "无法连接到服务器";	
	exit;
}
$contents = [System.Text.Encoding]::UTF8.GetString($byteContents);
$doc = New-Object -TypeName System.Xml.XmlDocument;
try
{
	$doc.LoadXml($contents);
}
catch
{
	Write-Host "无法解析服务器配置信息";	
	exit;
}
$wrt = $null;
try
{
	$conf = $doc.SelectSingleNode("/root/configuration");
	$isExists = [System.IO.File]::Exists($configFilePath);
	if($isExists -eq $true)
	{
		[System.IO.File]::Delete($configFilePath);
	}
	$wrt = [System.IO.File]::CreateText($configFilePath);
	$wrt.Write($conf.OuterXml);
	
}
catch
{
	Write-Host "写入配置文件失败";	
	exit;
}
finally
{
	if($wrt -ne $null)
	{
		$wrt.Close();
	}
}

try
{
	$regList = $doc.SelectNodes("/root/register/reg-item");
	foreach($reg in $regList)
	{
		$reKey = $reg.Attributes.GetNamedItem("path").Value;
		$reItem = $reg.Attributes.GetNamedItem("item").Value;
		$reValue = $reg.Attributes.GetNamedItem("value").Value;
		Set-RegistryValue $reKey $reItem $reValue;
	}
}
catch
{
	Write-Host "配置注册表信息失败";	
	exit;
}
Write-Host "完成初始化配置"
