<?

// > $last
// < $track

if(!file_exists("mania_server_tracks.txt"))
{
    echo "You need 'mania_server_tracks.txt' simple text track list file ! Server is disabled.";
    die("");
}

$tracks=file("mania_server_tracks.txt");

do
{
    $total = count($tracks);
    $last = rand(5, $total - 1);

    $track="".trim($tracks[$last]);
}
while($track=="");

?>

