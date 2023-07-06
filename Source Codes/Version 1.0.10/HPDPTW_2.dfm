object Form2: TForm2
  Left = 38
  Top = 229
  Width = 1288
  Height = 935
  Caption = 
    'A Heuristic Method for the Vehicle Routing Problem Using Simulat' +
    'ed Annealing'
  Color = clHighlight
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  WindowState = wsMaximized
  PixelsPerInch = 120
  TextHeight = 16
  object Button2: TButton
    Left = 613
    Top = 784
    Width = 137
    Height = 57
    Caption = 'Close'
    TabOrder = 0
    OnClick = Button2Click
  end
  object GroupBox2: TGroupBox
    Left = 13
    Top = 375
    Width = 412
    Height = 482
    Caption = 'Parameters'
    Color = clInfoBk
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 1
    object Label1: TLabel
      Left = 19
      Top = 33
      Width = 158
      Height = 20
      Caption = 'Initial Temperature'
    end
    object Label2: TLabel
      Left = 23
      Top = 89
      Width = 169
      Height = 20
      Caption = 'Ending Temperature'
    end
    object Label3: TLabel
      Left = 25
      Top = 137
      Width = 108
      Height = 20
      Caption = 'Cooling Rate'
    end
    object Label4: TLabel
      Left = 26
      Top = 201
      Width = 349
      Height = 20
      Caption = 'Maximum Temperature Change Iterations:'
    end
    object Label5: TLabel
      Left = 26
      Top = 257
      Width = 211
      Height = 20
      Caption = 'Maximum Total Iterations'
    end
    object Label6: TLabel
      Left = 31
      Top = 313
      Width = 123
      Height = 20
      Caption = 'Distance Limit'
    end
    object Label7: TLabel
      Left = 32
      Top = 377
      Width = 160
      Height = 20
      Caption = 'String length Limit:'
    end
    object Label8: TLabel
      Left = 35
      Top = 433
      Width = 121
      Height = 20
      Caption = 'Critical ration:'
    end
    object Edit1: TEdit
      Left = 288
      Top = 32
      Width = 97
      Height = 28
      TabOrder = 0
      Text = '4'
    end
    object Edit2: TEdit
      Left = 288
      Top = 88
      Width = 97
      Height = 28
      TabOrder = 1
      Text = '1'
    end
    object Edit3: TEdit
      Left = 288
      Top = 136
      Width = 97
      Height = 28
      TabOrder = 2
      Text = '0.999'
    end
    object Edit4: TEdit
      Left = 288
      Top = 192
      Width = 97
      Height = 28
      TabOrder = 3
      Text = '1000'
    end
    object Edit5: TEdit
      Left = 288
      Top = 304
      Width = 97
      Height = 28
      TabOrder = 4
      Text = '10'
    end
    object Edit6: TEdit
      Left = 288
      Top = 376
      Width = 97
      Height = 28
      TabOrder = 5
      Text = '3'
    end
    object Edit7: TEdit
      Left = 288
      Top = 432
      Width = 97
      Height = 28
      TabOrder = 6
      Text = '0.01'
    end
    object Edit8: TEdit
      Left = 288
      Top = 248
      Width = 97
      Height = 28
      TabOrder = 7
      Text = '100000'
    end
  end
  object GroupBox4: TGroupBox
    Left = 760
    Top = 24
    Width = 497
    Height = 329
    Caption = 'Objective Function'
    Color = clBackground
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 2
    object Label9: TLabel
      Left = 16
      Top = 65
      Width = 319
      Height = 20
      Caption = ' Weight of Travelling Time Per Second'
    end
    object Label10: TLabel
      Left = 24
      Top = 121
      Width = 310
      Height = 20
      Caption = 'Weight  of Waiting Times Per Second'
    end
    object Label11: TLabel
      Left = 24
      Top = 185
      Width = 273
      Height = 20
      Caption = 'Weight  of Lateness  Per Second'
    end
    object Edit9: TEdit
      Left = 352
      Top = 120
      Width = 97
      Height = 28
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      Text = '0.5'
    end
    object Edit10: TEdit
      Left = 352
      Top = 56
      Width = 97
      Height = 28
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      Text = '1'
    end
    object Edit11: TEdit
      Left = 352
      Top = 184
      Width = 97
      Height = 28
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      Text = '10'
    end
  end
  object GroupBox1: TGroupBox
    Left = 16
    Top = 24
    Width = 409
    Height = 329
    Caption = 'Files'
    Color = clTeal
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 3
    object Label12: TLabel
      Left = 56
      Top = 48
      Width = 123
      Height = 20
      Caption = 'Input Data File'
    end
    object Label13: TLabel
      Left = 56
      Top = 104
      Width = 93
      Height = 20
      Caption = 'Output File'
    end
    object Label14: TLabel
      Left = 56
      Top = 208
      Width = 100
      Height = 20
      Caption = 'Output Tour'
    end
    object Label15: TLabel
      Left = 56
      Top = 160
      Width = 113
      Height = 20
      Caption = 'Best Solution'
    end
    object Edit12: TEdit
      Left = 240
      Top = 48
      Width = 137
      Height = 28
      TabOrder = 0
      Text = 'rc107.out'
    end
    object Edit13: TEdit
      Left = 240
      Top = 96
      Width = 137
      Height = 28
      TabOrder = 1
      Text = 'rc107.out'
    end
    object Edit14: TEdit
      Left = 240
      Top = 152
      Width = 137
      Height = 28
      TabOrder = 2
      Text = 'BestTour.txt'
    end
    object Edit15: TEdit
      Left = 240
      Top = 208
      Width = 137
      Height = 28
      TabOrder = 3
      Text = 'Tour.txt'
    end
  end
  object GroupBox3: TGroupBox
    Left = 765
    Top = 376
    Width = 492
    Height = 465
    Caption = 'Initial Basic Solution'
    Color = clSilver
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 4
    object RadioButton1: TRadioButton
      Left = 16
      Top = 104
      Width = 305
      Height = 17
      Hint = 'The nearest job for each vehicle will be chosen'
      Caption = 'Insertion Heuristic'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object RadioButton2: TRadioButton
      Left = 16
      Top = 152
      Width = 425
      Height = 17
      Hint = 'The Vehicle and Job  wiil be selected randomly'
      Caption = 'Random Initial Feasible Solution'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object RadioButton3: TRadioButton
      Left = 16
      Top = 200
      Width = 345
      Height = 17
      Caption = 'Partition Insertion Heuristic'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
    object RadioButton6: TRadioButton
      Left = 16
      Top = 248
      Width = 265
      Height = 17
      Caption = 'Sweep Heuristic'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
    end
    object RadioButton7: TRadioButton
      Left = 19
      Top = 296
      Width = 414
      Height = 17
      Caption = 'Partition Insertion Heuristic'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
    end
    object RadioButton11: TRadioButton
      Left = 19
      Top = 56
      Width = 422
      Height = 17
      Hint = 
        'The length of  Tour for each vehicle equls to  Total numbe rof J' +
        'obs devided by number of vehicles'
      Caption = 'Deterministic Initial Feasible Solution'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      TabStop = True
    end
  end
  object GroupBox5: TGroupBox
    Left = 432
    Top = 377
    Width = 313
    Height = 306
    Caption = 'Automated Guided Vehicles'
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 5
    object Label16: TLabel
      Left = 206
      Top = 202
      Width = 55
      Height = 16
      Caption = 'Number'
    end
    object Image3: TImage
      Left = 16
      Top = 25
      Width = 153
      Height = 124
      Picture.Data = {
        0A544A504547496D6167654E170000FFD8FFE000104A46494600010101004800
        480000FFDB004300080606070605080707070909080A0C150E0C0B0B0C191213
        0F151E1B201F1E1B1D1D2125302921232D241D1D2A392A2D313336363620283B
        3F3A343E30353633FFDB0043010909090C0B0C180E0E1833221D223333333333
        3333333333333333333333333333333333333333333333333333333333333333
        33333333333333333333333333FFC00011080077009703012200021101031101
        FFC4001F0000010501010101010100000000000000000102030405060708090A
        0BFFC400B5100002010303020403050504040000017D01020300041105122131
        410613516107227114328191A1082342B1C11552D1F02433627282090A161718
        191A25262728292A3435363738393A434445464748494A535455565758595A63
        6465666768696A737475767778797A838485868788898A92939495969798999A
        A2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6
        D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FAFFC4001F01000301
        01010101010101010000000000000102030405060708090A0BFFC400B5110002
        0102040403040705040400010277000102031104052131061241510761711322
        328108144291A1B1C109233352F0156272D10A162434E125F11718191A262728
        292A35363738393A434445464748494A535455565758595A636465666768696A
        737475767778797A82838485868788898A92939495969798999AA2A3A4A5A6A7
        A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3
        E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00F757
        B86FE003F134D134E7B27E67FC29BD314A3AD0171DE6DC7FB1F99FF0A50F707F
        E79FE67FC281D69FE94863375C7A47F99FF0A375C7A47F99FF000A96908A5702
        3DF71FF4CFFEFA3FE149E65C7A47F99FF0A90F1CF6158BAA78974ED31583CEAF
        28190A8771FC872693928ABB2A30949DA28D392E248F06468501E06E7C669E24
        B8F44FCCD7976A5AA1F13C61EF6DB6C51A811AB0DA4824E491DBEE83D6A8C5A8
        6AF6F0C3E5EA772584B8625BAAF1C62B8AAE3E309F2D8F469E5939C6F7B33D7F
        CCB8F48FF33FE14BBAE7FE99FF00DF47FC2BCC0788B5FB759B6DFF00985586D0
        C99C0E7AD5C3E31D6E132E45BCA150326E5C6EF63531CCE8BDEE2795D65B347A
        266E7FE99FE67FC28CDCFF00D33FCCFF008570D1F8F6FD3065D3A275F2B78647
        C64E3A53CF8FE576B748B4B3BE5018977E00CF2323BD6AB1F41FDA327976257D
        9FC51DBFFA57FD32FF00BE8FF8527FA4FF00D33FFBE8FF00854F486BB0E120DD
        71FF004CFF00EFA3FE155AEB51FB14912DC14064CEDC1EB8FF00F58ABD8AF3CF
        8977B25A5F68410901CCD9F7C797FE3431ADCF4282759D032F7A96B07C33334D
        60849ED5B983EB45C2C7189A85F01C5DC9FF000200FF004A99756BE03FD6A1FA
        A0AF2E4F88BA823B24BA3DBB15272526619C55B87E2397C6FD11F1DCADC0FF00
        0AE870F238D73F73D31759BC1D4407FE027FC6A65D72E4637410B7FBAC462BCF
        6DBC751DC4AB1AE8D7DBDB180AE8D9FE55A8DE2484464ADBCC26C1C424A92A7D
        C8247E19ACAA4A10F88DA942B547682B9D9AEBC40CBDB103B90E38AAD77E2FB2
        B6461E5BBCC3A46A41E7DF15C21D4EEF50F9E594C3195C88E33EDC64D354220F
        94632467DEBCCAD8E8AD208F66865B2BFEF59ABA8788F54D499937ADB41D913A
        9FAF6FE758F142892338077B60B31392DC773DE9CD261A911C6E3F41FCABCEA9
        8894DEACF5214634D5A28B2EA1372A8C0DAA7AF7F9AA8A8FDD8FFAE83FA55A2E
        0B38FF00657FAD555FB83FEBA0FE95C989A9EF9BD15EE939FF0096BF41FD695B
        EFBE7FB94D27FD67AF1FC8D2B1FDE49FEE572739AD84276AA1C6711F033EC288
        47EF6D863B7F514D9388FAFF00CB2F4F614E87FD7DAE7DBF98AA8CB607D7FAEE
        7B1D14669335F6A7C5064D798FC58FF908787BEB3FFED3AF4D3D6BCCBE2C7FC8
        43C3DF59FF00F69D0CA8EE75BE12FF0090727D2BA3AE73C25FF20E4FA57474C1
        EE7CA864617B700AB705FA8345B5C80CA31C1900EBED5553E29F8A4B0060D3DC
        9F5B45E6AE5BFC49F124B711C32691A536F61FF2ED8E33827AD6EEA2B6C73AA7
        3B9DA9548558931C498E420DA31EFDCD4F160300A063D0562F896468F4BF202F
        CF2CB1C6371C01939C93F4AAAA97EBE20930927D98CCF2302C554A6D0381FC5C
        D7CD2839C14E52D5DF7F2B7F99F5CE4A178463A69F8DCDE3A85B4325CABC815A
        24DCC0F5C62AD6FC807D6B9E834DB8DEEC66C07DA030E5D14291C93D7AE3A76A
        DCCE1463B571D7708A4A2EFF00D236A5CCDBBA1EC73939A1186FC67B7F4A6337
        06910FEF38EB8E95CF196A8D64B465856064939EC2989F747FD74FF0A6DBB066
        623A10314F40368FFAE9FE1585797BE54168487F8FEA3F91A737DE939FE1A9A1
        2C8970CA70C10608EDC8A56B898171E63703BF35CFCC377BE8549BFD536307F7
        5DFBF029F0FF00C7C5AFD07F3153CF24AD0A85F2CB3447EFA02339EF4B04DFE9
        106F8E2202E4E2319E0678FCAB48C9684B6ECDD8F5856EC7A8FD6986E00671FD
        DEB5CA5BF89AE2ED14C165A9907EE97B375FE718AB726A8CBE59B9CC723E130E
        A7272338E3DC57DD7334D26B7F4F53E2F4B68CE823977F6E7AD797FC53130D77
        4566FF0052565D9CF43F2678FCABB8B4BA1F6733482455472003FC55C27C4B98
        CD7DA012C09067CE3B7FABAB628BBB476FE12FF90727D2BA3AE73C25FF0020E4
        FA5747416F73E40D3BCA92F62D910E2490918CF15D058BC571ACAA7D9C0600A0
        DC00C619B9AE021D5EE209565444C824E3279CD77BE0D0DA84136B5773C76B1C
        0C502E06D938392589E3938A9C5578D3A4E4FE5EBD0295294EA247517012E71E
        722C8A42E430E38031C7E54F470AC09200EE49C0ACA7D6ACE2DBBEEAD9720637
        4B9CFD3D6AD41A969F35BCDE75DDBA395FDD6ECED2475CFA8AF9B786C427CCE1
        26AFD99F50F158751B73ABFAA2DA301BF736092319EE7D2A4C8AA29A9D8BE90E
        EB77025EAB6E6CA92A467A7523F1CFF3ACC97C45651B61F51F2CF51909FE0697
        D4313536A6F4F26258EC327AD45F79D1311823DE98CEE8A5A350CE07CA0F4AC4
        8F59B19D5DD35296489582975240C9EDC250352D3C9FF8FB918918C99E4FE8B5
        70CAF1774D41FDC4CF32C25ACE6BF13A3B38E5F9F747C85FE10715652DE6D8BF
        B9931BFF00BB5A36B3C4F6B098E55752830CA490DC7AD4F35FDB59DA99677214
        31078EF8AF02AD6A92A8D72EA6F0ADA251466C602ADC8955800A323183D450DF
        66DD27320E39E9C557BCD3759D42E750BAB2D592DEDEE6D025AC4D1EEF2A4C83
        BCFE00FAF5F6AE5EF343F1DE9D63717971E28B2F2618DA490F91B8ED039E3673
        5B52A74E7A3A893ECEFF00E4672C572BD62FF0FF0033B3758088B2EE0797C7CA
        3A668B65852EE0712B7CA33829D45731A76BCB05A5A0D57528AE9E4865C4D0C2
        543796EDBC8F97918DA07A9538ABD6BE27D1649E22B76814C89086646C12E320
        74E38FCA9BC3D585ED16ED7D931AC552947E2B5FD0EE23F185847E73CB3BB853
        942A87014F63F8E7F4AC4D4BC69A7C3F64BBCACEDE71650AD81850C31CFB914B
        A53691A85AADD69F6F115C8C48612A4F19046403820F06A8F8C3483A8F872686
        CE287CE47575C80B800E4F35EDD3CF294F130A738B8EBAB7656DD76F33C19602
        6A9B69A7A74B99B7BE3F91A297C98E2505CED0F20623383803EB9E6B2350D7AE
        35ABEB04B868D8DB96C15EA776DEA7F0AE16F619AC6710CAD1B3140D98DC38C1
        19EA2B43C392F99A8007B115F5F5A84152738BB9E653528D4B48FA4FC25FF20E
        4FA57475CE784BFE41C9F4AE8EB84E87B9F0A81EA2BD35747934BF874D249179
        6162139E32F2CAC46DC8EC8B9E9DF9AE1A5D126191B25DDECC2BD26EF5EB2BAB
        15B196FD3ECF1BDAFF00CB3E655054C83DB18C7E15CD8E5888B83A51BD9EBE5F
        D7E1BEFA9B51749DD49F42BD9782F4DD7F48D3B506B9B88663085708171B8310
        4E3EB560F83EC6322393539613B8AAACB181BB1D48F51CD68F87351D3A2D0214
        8AEE2213CD6543200E57CC7238F523156751B09AFC5BDC5BA329881742B81F39
        65C6477E0366BC986658B55E719D6715776D179DB75E8763C3D19534D42FA799
        9CBE0688A031EA60871C1F2C7CDF4E79ACABFF00863737336F4D4E20318C1423
        22BB9D32178F4DB58E64DB2C69820F6E4D5DC1FC73F9570D5E20CC21371F6974
        9F68FF0091AC32EC359351B3F5679BC1E01B8D16CEEAEAE751B768A246919590
        918039F4E6B0FC3B7926B5AB436874FB3844C098D9F20391CEDCF62457A178F9
        997C11A810C5784071DC6E1C1AF315B49F42B8B2BA89DFED513C32C85DB20B3E
        0E00ED8040FCFD2BDBCB335C5E22849CA7EF6B6D1745D7E6CE2C5E0E8C2A2B2D
        3AEA7AAF855E0D33C413F87AF91D6390EFB462DC1079C035D46BFE12D3B5AD2E
        4B5B8462919DEB9623040F6F6AC7D5B4E4D4634F98C53C443C3328E636FF000F
        515D0E91ADACD6C13502B6F77180240C787F753DC1AF2A75618EB622946D557C
        496FFE25FAF67EB734519611BA7395E0F66FF27FA16F4AD1A0834CB785B7131A
        051838E074A9EEF40D3EFAD26B5B88DDE1990A48BB88CA91C8A72EA9649926EA
        103BFCC2AA5C789ECA11FBB124CDD82AE01FC4D4D0CB6751AB526DF7B7EA4D6C
        C2942EE5515BD4CBBEF00F869219AE1EC1D9A3566554948C124B10BD8649FD6B
        92D2F4EF06EAAA629F44D52CEECCA235B564932C4B0F981031C75393D01AEBAE
        3C5178E4F9500890772371FE82A88D6AFD66F345C9DE7B6063F2C57B74726C55
        BDEB2F9BBFE1A1E6CB3BC2C5ECDFF5E6743A678474CD22DDA0B369D6366DD869
        0B630000067A00074A7EA1E1C8AF34EBAB659E4569A2640738C12315976FE29B
        A52A2586397279C7CA7FAD6AC3E25B59302549613EACB91F9D70D6C8EAC65CF2
        A777DD6BFF0004E8A39C5192B29DBD4F9D356D06E2DF55B8B58B4ED55960FDDE
        E643202E3A9CFF0077D2A5F0DC13DBDF849ED26B7391FEB632A5BD7AF5AFA185
        FD9497AAA93C4DF68FBA33825876C7B8FE55C7FC45006A1A1E0779BFF69D7B54
        6ACE54529269ECD3FEBE666E49D5BA69A677BE12FF0090727D2BA3AE73C25FF2
        0E4FA57475453DCF99C58452BF96E1481D981CD3752F0CE9B7081FC8DAC48E50
        915BE9A72AED777CB8EE6A49A06284065231C56D53539294AC79A5C780D8C464
        B7BA2AE7276BFF008D4213C5FA2227D9AEE5088B81E53678FA57A249010B8046
        7D85549A361C107F2EB5CB2829692574762935AA672F0FC50D76C3CA49EDD666
        55C48271F78FA8C018ADEB2F8C1A74800BDD3AE216F589838FD714E9F4DB7B94
        C491237D4035CE6A3E0A86425ED98A1CFDDC0C579D532FC1D4F8A16F43A6389A
        D1DA4755E2BF1668FAEF816EFEC37637492AC6B1C836B9230C78FA77ACDF886B
        690E95A56AF68C33796B1315CE4165C1C91ED8238F7AE0EFBC337B6203F079E3
        1D6AA5FCF78F0C50CC922C683E45249033D703B66B7C26029E1D7B8F4D7F1B7F
        9222B579547EF2FEBFA67BEF873C49FF00090E8516A9F62785598A14570FCAF0
        4F415ACBAA58E70F30523B488463F4AF0DF0CFC44D47C35A72E9E9676F716A8C
        5806CAB649E79FFEB574ABF1674E987FA4E8D3AB7FD339030FD715F3988C9AB2
        A92F670F76FA59F4F99E8D3C653714A4F53D3D753D31739BC8320F7619AC8B93
        6734732C77BA7C523472A899657DCE5BA6EE7031ED5E7737C47D21D99934EBBE
        4F72B5037C44D3BF874CB83F5715B50C0E3295F96FAFA133AD87934DDB43B3D1
        F4D161AB5B5DDCEBD69245113B916573BB823A1E2BAD3AD6919FF8FB807B035E
        367E225A76D2E4FC651FE15D8F852FB49F15DBBB4571F65B88FEFC3281FA1CF3
        575B2DC5E22579DFEF40F17416AEDF77F923A7D5B52D32F74AB9B58B52861925
        5DAB200DF2F3EDF4AE5B466B1B0D60C92789ACE76F2DA316FF00BC39638C6413
        EDD05758BE0FB7931FE9CA41E988F35A9A2785ACB49BF7BBDD14F2326D01A15F
        507393CF6AE8C2E5D89A6B9399A4FD19955C6D0E476B3F915FC3C23BBBC9A58E
        48A53E7F9C4451B22443006177127A8F5EF547E21A6DD47443B98E4CDD5B38FB
        95D4E85A4FF63DA988BABB1CE5C0E4F24F3F9D72FF00110E750D0CFBCDFF00B4
        EBD8C361DD08494A576DDDB3CD9D5552A2715648EF7C25FF0020E4FA57475CE7
        84FF00E41E9FEED7455D037B9E51ABE8B36964F961AE3EBF2FF8D73EF777BC81
        A71FAF99FF00D6AF6EB8D3E1B8CEF50735C3F88355B2D17C5BA3E8FF0061F362
        BBC1BB9F1C5B07611C47D3E6938E7D29F348854E0BA1C13CB7ADD2C187FDB4FF
        00EB544E2FDFFE5C88FF00817FF5ABD0F56F10786F4CB1BA9D629A79A17B9896
        18E16CBC902EE719C1C0C77E9F95374EF11F866F1B4DB7984D0DE5E470EE46B5
        902432483E4476DB852DCED048C8C1EE2A6ECBB23CEFCABEEF6473ECDFFD6A69
        B7BE3FF2E67F06FF00EB577EBE33F051620CB74AA392ED632EDDBBCA16DDB71B
        437CA5BA0240EF49E23F16786F42835444827B8BEB189D8442DA4092BA6DDCAA
        FB7076EE52D8CE0126A1C13DCAE6B1E6B7BA4DEDEA8536E57073D7359B7BE11B
        9BD0A24888DA30302BDB46B5E1617E9632C925BCCC819BCFB692358D8C7E688D
        D88DA8FB32DB09CF150787B57D23C4DE21BAB2D3A063670D9457225961789D8B
        B38E0301952AAAC08EBBAAA31E556426D3DCF0C7F87CEC9811B86FEF73550FC3
        6BBC9C4CC3FED9FF00F5EBDC6C754B8BED42CEE23D334F5D12F2F1ED6067BC0B
        72E158A194210015DC31B41DDC8356752D7F408F457BCB0789A56B692E22FB44
        6EA8523996272768247CCC00E29EA1A1E067E1A5EFFCFC1FFBF5FF00D7A3FE15
        A5EFFCFC9FFBF3FF00D7AF78BDF12E810789AD348870C9E7CF15ECEF13AC7108
        A1791F6BE36B152AA1803C66AC2F89BC1ED662E04B7059A658560FB14BE73165
        2E088F6EE2A5016DD8C628D4343E7E1F0D2F47FCBC9FFBF5FF00D7AB36BF0FF5
        0B4937C770D9F78BFF00AF5EDD6BABC73784340F10C96300B7BFBA8E1BAC1388
        51D8A2B83FEF6CCE7B31A583C4FE1F8AE2EC6A40411ADECB0DBB470BC998A361
        1B4CE541089E66E504E07BF5A350D0F33D36CBC43A5B66DA7DA3D3CBE3F9D755
        6BE24D76240B73631DC1F5C95FF1ADB8BC53A4BDB6BB1491C706A3A79BF312CB
        14822916DD987DF0304E02960B9201E957351F11787ACE0BC8623E66A105AC92
        2810C9E4B4AB0997CAF331B776D19C6738F7A7CD223D9C3B18B0F8BB528C7CDA
        367D36DC11FCC1AA9A95C5E789AF2C59EC0DBFD9B775937EEDDB7D863EED75DA
        76B1E1DBCBBB1D3E46D9A85CC5196510B9896478FCC11F998DA1B6E4852738AE
        A63D0ED636C845CFD295D8D46117748AFE1DB66B7B15561CE2B6B1ED491C6B1A
        ED51C53E8B0EE15C36B5F0D74FF105D6B57B7F7972D7B7C156DE48A59235B554
        50231B03E1C87CBFCDDCF415DCD317FC2988F3C6F8677536A134B73E20DF6D2B
        DDC8605B20A43DCC46390EEDE78C9C818F6E7AD4B69F0E6F2278E3B8F1009ACA
        592DA5BE816C827DA1EDF688F6B6F3B1711C608E73B782335DF9EB483B5033CF
        5FE17674C6B2FED8FBD61259EFFB37F7EE7CFDD8DFDBEEE3F1CF6AADA97C257D
        535AD4F50B8D7D985EA5CA286B52D246B30C05DE5F954E30001C0C7BD7A61E9F
        8FF5A0741F4A4079DCBF0B23BDD727D4750D4A19D6EC6EBB55B0457690C5E592
        8E598C6A78381920FF001609A9B4FF0086713DC893C47A826B4915B5BC1027D9
        CDBF97E4972872AE727E739CF7CFD077DFC340EB4C4706DF0E98EA965FF13641
        A4596A1F6EB6B43648D2C2FE67985166CE421719C6DCE0633544FC2CB8682EAD
        9BC43BAD1AD67B5B68CD90061592E12624B6FF009CE571DBAF6C73E907FAFF00
        8D0BF74D0070377F0DA4BDD466136BAC74A92E2F271642D46F56BA8A447FDE6E
        EC646238EF8F71069DF0BA7D292DAE2CB5B82DF55B79F7C77316988A9B3CB285
        59037CC4824EE2DC1E800E2BD10FDF3F514A3A8FA7F4A0672F6FE0B8A2F8747C
        23717867436CD0FDABCBDA4312487DB9EA0E0F5ED58179F092D6E2DF488D3504
        0D6769F669DE7B249BCF1BF7B3A8638462CCE72777DEE8715E90DD3F2FE747F7
        7E9408E12FBE1CFDB209E3FED6D8653A99CFD9B38FB667FDAFE0CFFC0B1DA9C7
        C017ABFDA56D6DE217874BBE595A5B416A0932BC3E5162FBB257F8B6FAF7AEE0
        F51F87F5A55E8DF5340CE0B4EF865069DE27B6D5D6FA278E231BB44F648D2191
        2211E56424955380D80339E84035E814CEFF008D3E81051451401FFFD9}
    end
    object Label34: TLabel
      Left = 21
      Top = 170
      Width = 167
      Height = 16
      Caption = 'Time Window of Vehicle'
    end
    object Edit16: TEdit
      Left = 214
      Top = 234
      Width = 48
      Height = 24
      Enabled = False
      TabOrder = 0
      Text = '2'
    end
    object Edit17: TEdit
      Left = 198
      Top = 163
      Width = 48
      Height = 24
      TabOrder = 1
      Text = '2'
    end
    object UpDown2: TUpDown
      Left = 249
      Top = 160
      Width = 19
      Height = 25
      Hint = 'Increase or Decrease '
      Enabled = False
      Min = 10
      ParentShowHint = False
      Position = 10
      ShowHint = True
      TabOrder = 2
      Wrap = False
    end
    object GroupBox6: TGroupBox
      Left = 30
      Top = 197
      Width = 155
      Height = 100
      Caption = 'Capacity'
      TabOrder = 3
      object RadioButton4: TRadioButton
        Left = 16
        Top = 34
        Width = 121
        Height = 16
        Caption = 'One Container'
        TabOrder = 0
      end
      object RadioButton5: TRadioButton
        Left = 16
        Top = 66
        Width = 133
        Height = 16
        Caption = 'Two Containers'
        Checked = True
        TabOrder = 1
        TabStop = True
      end
    end
  end
  object GroupBox7: TGroupBox
    Left = 432
    Top = 25
    Width = 313
    Height = 328
    Caption = 'Quay Cranes'
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 6
    object Label18: TLabel
      Left = 22
      Top = 90
      Width = 119
      Height = 16
      Caption = 'Number Of Crane'
    end
    object Label42: TLabel
      Left = 18
      Top = 36
      Width = 139
      Height = 37
      Alignment = taCenter
      AutoSize = False
      Caption = 'Quay Crane Time Window (Second)'
      Color = clInfoBk
      ParentColor = False
      WordWrap = True
    end
    object Label19: TLabel
      Left = 30
      Top = 274
      Width = 167
      Height = 16
      Caption = 'Number  of Jobs (Static)'
    end
    object Edit18: TEdit
      Left = 206
      Top = 82
      Width = 48
      Height = 24
      Enabled = False
      TabOrder = 0
      Text = '10'
    end
    object Edit19: TEdit
      Left = 206
      Top = 43
      Width = 48
      Height = 24
      TabOrder = 1
      Text = '120'
    end
    object GroupBox20: TGroupBox
      Left = 23
      Top = 120
      Width = 266
      Height = 129
      Caption = 'Ship Handling'
      Color = clScrollBar
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      TabOrder = 2
      object RadioButton8: TRadioButton
        Left = 6
        Top = 30
        Width = 133
        Height = 16
        Caption = 'Single   Crane'
        Checked = True
        TabOrder = 0
        TabStop = True
      end
      object RadioButton9: TRadioButton
        Left = 6
        Top = 51
        Width = 212
        Height = 33
        Caption = 'Multiple Cranes (Random)'
        TabOrder = 1
      end
      object RadioButton10: TRadioButton
        Left = 6
        Top = 81
        Width = 198
        Height = 33
        Caption = 'Multiple Cranes (Circular)'
        TabOrder = 2
      end
    end
    object Edit20: TEdit
      Left = 230
      Top = 266
      Width = 48
      Height = 24
      Enabled = False
      TabOrder = 3
      Text = '5'
    end
  end
  object Button3: TButton
    Left = 454
    Top = 704
    Width = 131
    Height = 57
    Caption = 'Generate Data'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 7
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 608
    Top = 704
    Width = 137
    Height = 57
    Caption = 'Write Best Tour'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 8
    OnClick = Button4Click
  end
  object Button1: TButton
    Left = 457
    Top = 784
    Width = 131
    Height = 57
    Caption = 'Execute'
    TabOrder = 9
    OnClick = Button1Click
  end
end