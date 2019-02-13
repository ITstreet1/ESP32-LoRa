<?php
include "../include/connect_to_mysql.php"; // php fajl preko koga ćemo se konektovati na bazu podataka
$output = '';
$chartOutput='';
if(isset($_GET['temp']) && !empty($_GET['temp']) AND isset($_GET['vlaga']) && !empty($_GET['vlaga'])AND isset($_GET['hygro']) && !empty($_GET['hygro'])){
	$temp=strip_tags(stripslashes(mysqli_real_escape_string($con,$_GET['temp'])));
	$vlaga=strip_tags(stripslashes(mysqli_real_escape_string($con,$_GET['vlaga'])));
	$hygro=strip_tags(stripslashes(mysqli_real_escape_string($con,$_GET['hygro'])));
	mysqli_query($con,"INSERT INTO test SET sensor = 'DHT22', temp = '$temp', vlaga = '$vlaga', hygro = '$hygro', date=now()");
}
$sql = mysqli_query($con,"SELECT * FROM test WHERE sensor='DHT22' ORDER by date ASC") or die(mysqli_error());
$count = mysqli_num_rows($sql);
if($count > 0){
     while($row = mysqli_fetch_array($sql,MYSQLI_ASSOC)){
             $id = $row["id"];
			 $value = $row["temp"];
			 $value2 = $row["vlaga"];
			 $value3 = $row["hygro"];
			 $date = strftime("%X", strtotime($row["date"]));
			 $output .="['".$date."',".$value.",".$value2.",".$value3."],";
			 $chartOutput = rtrim($output, ",");
	 }
}
?>

<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Arduino - no limits</title>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
          ['Vreme', 'temp', 'vlaga', 'hygro'],
          <?php echo $chartOutput; ?>
        ]);
        var options = {
          title: 'Očitavanje temperature van lokalne mreže',
          curveType: 'function',
          legend: { position: 'bottom' }
        };
        var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));
        chart.draw(data, options);
      }
    </script>
</head>

<body>
<div id="curve_chart" style="width: 1900px; height: 500px"></div>
</body>
</html>