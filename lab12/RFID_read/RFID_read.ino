
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN      A1        // reset pin
#define SS_PIN       10        // select pin for the chip

MFRC522 mfrc522(SS_PIN, RST_PIN);  // create MFRC522 object
MFRC522::MIFARE_Key key;

byte blockData[16] = "Hello World!";   // max write data length is 16
byte clearBlockData[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//byte myUID[4] = {E9,42,8B,AB};

struct RFIDTag {    // 定義結構
   byte uid[4];
   char *name;
};
struct RFIDTag myUID = {  // 初始化結構資料
{0xE9,0x42,0x8B,0xAB}, "MyCard"
};
// use buffer size 18 to store the value
byte buffer[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

MFRC522::StatusCode status;

void writeBlock(byte _sector, byte _block, byte _blockData[]) {
    if (_sector < 0 || _sector > 15 || _block < 0 || _block > 3) {
        // check the validation of sector and block
        Serial.println(F("Wrong sector or block number."));
        return;
    }

    if (_sector == 0 && _block == 0) {
        // first block is read-only
        Serial.println(F("First block is read-only."));
        return;
    }

    byte blockNum = _sector * 4 + _block;  // get the real block number（0~63）
    byte trailerBlock = _sector * 4 + 3;   // get the trailer block

    // key validation
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // write the data to the block
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockNum, _blockData, 16);
    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    Serial.println(F("Data was written."));
}

void readBlock(byte _sector, byte _block, byte _blockData[])  {
    if (_sector < 0 || _sector > 15 || _block < 0 || _block > 3) {
        // check the validation of sector and block
        Serial.println(F("Wrong sector or block number."));
        return;
    }

    byte blockNum = _sector * 4 + _block;  // get the real block number（0~63）
    byte trailerBlock = _sector * 4 + 3;   // get the trailer block

    // key validation
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    byte buffersize = 18;
    // read the data from the block and save the result to _blockData
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum, _blockData, &buffersize);

    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    Serial.println(F("Data was read."));
}

void printRFIDdata() {
    Serial.print(F("Read block: "));
    for (byte i = 0 ; i < 16 ; i++) {
        Serial.write(buffer[i]);
    }
    Serial.println();
}

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();   // init MFRC522
    // init the key with 0XFF
    for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
    }
    Serial.println(F("Please scan MIFARE Classic card..."));
}

void loop() {
    // check if a new card is valid
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        byte *id = mfrc522.uid.uidByte;   // get UID of the card
        byte idSize = mfrc522.uid.size;   // get UID length
        Serial.print("PICC type: ");      // show card type
        // according to the SAK value (mfrc522.uid.sak) in response , identify the card type
        //SAK:select acknowledge
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));
        Serial.print("UID Size: ");       // show UID length
        Serial.println(idSize);

        for (byte i = 0; i < idSize; i++) {
            Serial.print("id[");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(id[i], HEX);       // show UID value in hexdecimal
        }
        Serial.println();

        writeBlock(15, 0, blockData);  // write sector 15 and block 0 with blockData
        // writeBlock(15, 0, clearBlockData);  // clear the data in the sector 15 and block 0

        readBlock(15, 0, buffer);      // Read sector 15 and block 0 and store to buffer.
        printRFIDdata();

//      bool foundTag = false;            // 是否找到紀錄中的標籤，預設為「否」。
            //Compare two array, array1's ptr, array 2's ptr, size_t.
        if (memcmp(myUID.uid, id, idSize) == 0) { //Same, <0 : First < Second, and vice versa.
          Serial.println(myUID.name);  // 顯示標籤的名稱
          Serial.println("Correct!");  // 顯示標籤的名稱          
//          foundTag = true;  // 設定成「找到標籤了！」
        }
        else{
          Serial.println("Wrong!");
        }
 
//      if (!foundTag) {    // 若掃描到紀錄之外的標籤，則顯示"Wrong card!"。
//        Serial.println("Wrong card!");
//      }



        // halt the card
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD, then you can read or write the sector and block many times
        mfrc522.PCD_StopCrypto1();
    }
}
