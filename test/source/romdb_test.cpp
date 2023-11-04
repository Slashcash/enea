#include "romdb_mock.hpp"

#include <gtest/gtest.h>

class RomDBFixture : public ::testing::Test
{
 protected:
    RomDBMock romdb;

    inline RomDBFixture() : romdb(DB_PATH)
    {
    }

    static const std::filesystem::path DB_PATH;
    static const std::string XML;
};

const std::filesystem::path RomDBFixture::DB_PATH = "db.xml";
const std::string RomDBFixture::XML = R"(
<datafile>
	<header>
		<name>MAME</name>
		<description>MAME 0.106</description>
		<category>Emulation</category>
		<version>0.106</version>
		<date>-not specified-</date>
		<author>AntoPISA</author>
		<email>progettosnaps@gmail.com</email>
		<homepage>http://www.progettosnaps.net/</homepage>
		<url>-not specified-</url>
		<comment>-not specified-</comment>
		<clrmamepro/>
	</header>
    <game name="sf2" sourcefile="cps1.c">
		<description>Street Fighter II - The World Warrior (World 910522)</description>
		<year>1991</year>
		<manufacturer>Capcom</manufacturer>
		<rom name="sf2_05.bin" size="524288" crc="ba529b4f" sha1="520840d727161cf09ca784919fa37bc9b54cc3ce"/>
		<rom name="sf2_06.bin" size="524288" crc="22c9cc8e" sha1="b9194fb337b30502c1c9501cd6c64ae4035544d4"/>
		<rom name="sf2_07.bin" size="524288" crc="4b1b33a8" sha1="2360cff890551f76775739e2d6563858bff80e41"/>
		<rom name="sf2_08.bin" size="524288" crc="57213be8" sha1="3759b851ac0904ec79cbb67a2264d384b6f2f9f9"/>
		<rom name="sf2_09.bin" size="65536" crc="a4823a1b" sha1="7b6bf59dfd578bfbbdb64c27988796783442d659"/>
		<rom name="sf2_14.bin" size="524288" crc="14b84312" sha1="2eea16673e60ba7a10bd4d8f6c217bb2441a5b0e"/>
		<rom name="sf2_15.bin" size="524288" crc="2c7e2229" sha1="357c2275af9133fd0bd6fbb1fa9ad5e0b490b3a2"/>
		<rom name="sf2_16.bin" size="524288" crc="5e9cd89a" sha1="f787aab98668d4c2c54fc4ba677c0cb808e4f31e"/>
		<rom name="sf2_17.bin" size="524288" crc="b5548f17" sha1="baa92b91cf616bc9e2a8a66adc777ffbf962a51b"/>
		<rom name="sf2_18.bin" size="131072" crc="7f162009" sha1="346bf42992b4c36c593e21901e22c87ae4a7d86d"/>
		<rom name="sf2_19.bin" size="131072" crc="beade53f" sha1="277c397dc12752719ec6b47d2224750bd1c07f79"/>
		<rom name="sf2_24.bin" size="524288" crc="c1befaa8" sha1="a6a7f4725e52678cbd8d557285c01cdccb2c2602"/>
		<rom name="sf2_25.bin" size="524288" crc="994bfa58" sha1="5669b845f624b10e7be56bfc89b76592258ce48b"/>
		<rom name="sf2_26.bin" size="524288" crc="0627c831" sha1="f9a92d614e8877d648449de2612fc8b43c85e4c2"/>
		<rom name="sf2_27.bin" size="524288" crc="3e66ad9d" sha1="9af9df0826988872662753e9717c48d46f2974b0"/>
		<rom name="sf2_29a.bin" size="131072" crc="bb4af315" sha1="75f0827f4f7e9f292add46467f8d4fe19b2514c9"/>
		<rom name="sf2_36a.bin" size="131072" crc="c02a13eb" sha1="b807cc495bff3f95d03b061fc629c95f965cb6d8"/>
		<rom name="sf2e.28g" size="131072" crc="8bf9f1e5" sha1="bbcef63f35e5bff3f373968ba1278dd6bd86b593"/>
		<rom name="sf2e.30g" size="131072" crc="fe39ee33" sha1="22558eb15e035b09b80935a32b8425d91cd79669"/>
		<rom name="sf2e.31g" size="131072" crc="69a0a301" sha1="86a3954335310865b14ce8b4e0e4499feb14fc12"/>
		<rom name="sf2e.35g" size="131072" crc="626ef934" sha1="507bda3e4519de237aca919cf72e543403ec9724"/>
		<rom name="sf2e.37g" size="131072" crc="fb92cd74" sha1="bf1ccfe7cc1133f0f65556430311108722add1f2"/>
		<rom name="sf2e.38g" size="131072" crc="5e22db70" sha1="6565946591a18eaf46f04c1aa449ee0ae9ac2901"/>
	</game>
</datafile>
)";

TEST_F(RomDBFixture, info)
{
    EXPECT_CALL(romdb, loadFromFile(DB_PATH)).WillOnce(testing::Return(std::make_pair(std::nullopt, XML)));

    EXPECT_FALSE(romdb.load().has_value());
    auto rom = romdb.find("sf2");
    ASSERT_TRUE(rom.has_value());
    ASSERT_TRUE(rom->name.has_value());
    EXPECT_EQ(rom->name.value(), "Street Fighter II - The World Warrior (World 910522)");
    ASSERT_TRUE(rom->year.has_value());
    EXPECT_EQ(rom->year.value(), "1991");
    ASSERT_TRUE(rom->manufacturer.has_value());
    EXPECT_EQ(rom->manufacturer.value(), "Capcom");
}

TEST_F(RomDBFixture, infoNonExistantRom)
{
    EXPECT_CALL(romdb, loadFromFile(DB_PATH)).WillOnce(testing::Return(std::make_pair(std::nullopt, XML)));

    EXPECT_FALSE(romdb.load().has_value());
    auto rom = romdb.find("lol");
    ASSERT_FALSE(rom.has_value());
}