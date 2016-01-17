# genesisextractor
Tool that automatically extracts official P2P port and genesisblock data out of daemons.<br>
This is part of <a href="http://www.spreadcoin.info" target="_blank"><b>Spreadcoin's</b></a> <b>"Altcoin Taxonomy Initiative"</b>.<br>
Read more about it here: <a href="http://spreadcointalk.org/index.php?topic=730.0" target="_blank">http://spreadcointalk.org/index.php?topic=730.0</a>

<h1><u><b>Download:</b></u></h1>

Win (32bit): http://www.spreadcoin.info/downloads/genesis-extractor.zip

<h1><u><b>Quick Guide:</b></u></h1>

Unpack the zip-file somewhere. Click on GenesisExtractor.exe to start the tool.

![Alt text](docs/pics/screenshot0.jpg "Genesis Extractor Screen 0 - Overview")

For the tool to be able to process daemons, it requires that you put atleast one altcoin-daemon in the /daemons folder.

![Alt text](docs/pics/screenshot1.jpg "Genesis Extractor Screen 0 - Overview")

You can add any daemon you like, and if you can't find a daemon for the coin of your choice you can try with the qt-wallet.exe.<br>
(Note: some coins come with a few external dll's they make use of. They simply won't work correctly without those external dll's).

![Alt text](docs/pics/screenshot2.jpg "Genesis Extractor Screen 0 - Overview")

Once there are daemons to process, they will appear in the "Available Daemons" list. <br>
(You might need to restart the tool to rescan for daemons you just added).<br><br>
You can now click "START EXTRACTION" to start the tool.

![Alt text](docs/pics/screenshot3.jpg "Genesis Extractor Screen 0 - Overview")

Please note that during the extraction operation you might see external windows appear and disappear.
<b>Don't interfere with this process.<b><br>Any process/window that is opened will also be closed automatically later.<br><br>

And it's possible that your firewall will ask you if you want to open the port that the coin is accessing.

<b>You don't need to open any port, since the extraction-tool will recognize what port the coin is trying to use, even when said port is closed.<b><br><br>
![Alt text](docs/pics/screenshot3b.jpg "Genesis Extractor Screen 0 - Overview")

![Alt text](docs/pics/screenshot4.jpg "Genesis Extractor Screen 0 - Overview")

![Alt text](docs/pics/screenshot5.jpg "Genesis Extractor Screen 0 - Overview")

![Alt text](docs/pics/screenshot6.jpg "Genesis Extractor Screen 0 - Overview")
