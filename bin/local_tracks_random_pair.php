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
    if(($last % 2)!=0 || $last == -1)
    {
        $tmp = rand(3, $total/2 - 1);
        $last = $tmp * 2;
    }
    else
    {
        $last++;
    }

    $track="".trim($tracks[$last]);
}
while($track=="");

?>
