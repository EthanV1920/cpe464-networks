make
# ./trace test_files/ArpTest.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/ArpTest.out
#
# ./trace test_files/IP_bad_checksum.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/IP_bad_checksum.out
#
# ./trace test_files/PingTest.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/PingTest.out
#
# ./trace test_files/TCP_bad_checksum.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/TCP_bad_checksum.out
#
# ./trace test_files/UDPfile.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/UDPfile.out
#
# ./trace test_files/largeMix.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/largeMix.out
#
./trace test_files/largeMix2.pcap > tmp.txt
diff --ignore-all-space -b tmp.txt test_files/largeMix2.out

# ./trace test_files/mix_withIPoptions.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/mix_withIPoptions.out

# ./trace test_files/smallTCP.pcap > tmp.txt
# diff --ignore-all-space -b tmp.txt test_files/smallTCP.out

