# DXXD Ransomware Decryptor
Decryption algorithm that decrypts files crypted by DXXD Ransomware that was spotted in 2016.
It was a type of malware that encrypted files on your computer and demanded ransom to decrypt them.  

## Idea
Idea was to analyze how spcific ransomware crypted files and what algorithms were used. 
## Build
Use included makefile to build executable with gcc. Compatible also with msvc.
## Usage
Run with requested arguments. Destination file will be decrypted file.
```cmd
DXXD_Ransomware_Decrypt_Algorithm.exe <path to crypted file> <path to destination file>
```
## Structure of crypted file
```
              / *   N  a t i o n a l   T r e n d s   C S S * /
     normal   2F2A204E 6174696F6E616C205472656E6473204353532A2F 0D0A0D0A
     crypted  2F2A204E A300D4385E9C9D92FE77B820750E492AED2A8732 BAB4085C
              ^        ^                                        ^
              |        Crypted with DXXD Ransomware             File endings
              Non crypted 4 bytes
```
## Example
```cmd
X:\DXXD_Ransomware_Decrypt_Algorithm>DXXD_Ransomware_Decrypt_Algorithm.exe "X:\\hotsites.cssdxxd" "X:\\hotsites-decrypted.css"
Decrypting ...

decrypted_bytes hex (first 200):
2f2a20486f74736974657320435353207374796c65732a2f0d0a0d0a696e7075745b747970653d746578745d207b20746578742d616c69676e3a2063656e7465723b77696474683a313030253b206d61782d77696474683a343070783b7d200d0a696e7075745b747970653d636865636b626f785d207b20706f736974696f6e3a72656c61746976653b20766572746963616c2d616c69676e3a626f74746f6d3b746f703a2d3370783b207d200d0a6c6162656c7b6d617267696e3a37252030253b646973706c61

decrypted_bytes text (first 200):
/* Hotsites CSS styles*/

input[type=text] { text-align: center;width:100%; max-width:40px;}
input[type=checkbox] { position:relative; vertical-align:bottom;top:-3px; }
label{margin:7% 0%;displa

File X:\\hotsites-decrypted.css written successfully!

Press key to exit
```
## How encryption here works
Generally we decrypt file by loading it as binary hex stream. By generating key stream the length of crypted file we assure that all bytes will be decrypted. Interesting thing is that first 4 bytes of crypted file are not encrypted. Firstly we rotate 3 bytes left and then xor with corresponding key.

* Crypto algorithms used:
  * xor  -> Well known bitwise operation.
  * rotl -> Rotate bits -> loseless method of shifting bits in circle.
* Each 4 crypted bytes use unique key that is chained together with previous 4 bytes.

