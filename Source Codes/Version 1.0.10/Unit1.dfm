object Form1: TForm1
  Left = 515
  Top = 208
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Clock'
  ClientHeight = 108
  ClientWidth = 107
  Color = clCream
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  Visible = True
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 16
    Top = 64
    Width = 5
    Height = 19
    Color = clWhite
    Font.Charset = ANSI_CHARSET
    Font.Color = clSilver
    Font.Height = -16
    Font.Name = 'Transformers'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
  end
  object Image1: TImage
    Left = 128
    Top = 32
    Width = 57
    Height = 25
    Cursor = crHandPoint
    Picture.Data = {
      0A544A504547496D616765DC040000FFD8FFE000104A46494600010102001C00
      1C0000FFDB0043000503040404030504040405050506070C08070707070F0B0B
      090C110F1212110F111113161C1713141A1511111821181A1D1D1F1F1F131722
      24221E241C1E1F1EFFDB0043010505050706070E08080E1E1411141E1E1E1E1E
      1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E
      1E1E1E1E1E1E1E1E1E1E1E1E1EFFC00011080019003803012200021101031101
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
      E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00FACF
      C4B7D716915A5BDB37952DEDC7D9C4D804C43CB772C01E09C21033C024120818
      3CEDD6A42D6E1A0BAF1D4B04CB8DD1C8D66AC3232320C59E95B7E2DB5BE9A2D3
      E7B1B5FB5C969782678448119D0C6F19DA5B8C8F33382467079AE7EF2168667B
      CBBD1F5FB14B89E249648F5211A6F62912B158A7FF0074640ED5206A783B537B
      ED5350B75D6FFB5ADE18207493311D8ECD286198D40E8ABC1FEB5D5D71A6FEF3
      4ED23C456F0DCCCEF63606F2D6595CC8D1EE4930A4BE4B61A22D9627EFE31851
      547579746D265B68AFB53D66392E98AC08BA85E397231C00AC79F987E75407A0
      551D4753D374D31FF68EA36967E667679F32C7BB18CE32467191F9D7290CE2C6
      F6CBFB3AEF519EEA7921DD6B35C4F39F21DC2BBBC6E4940A093B885C15009E4A
      94B3865B7F114ABA95B6A02F256BB7B69E6BAF3223079EA708BBCECF95A2E36A
      F4F6A00EC6CEEADEF6D52E6CEE21B981F3B6489C3AB60E0E08E0F208A2B8FD06
      DEE25D72D6EB4DB3D561B24BDB9FB4C8F7B9B77C0955B1179A7ACB83F707AF14
      5006EF8C34B5D57476885A45753DB38B88219141591D73F21DDF2FCCA597273B
      776EEA0573D16911432A4B17805A39118323AC7660A91D083E6706BBDA280390
      5D2B52BFB4D78BDA3593EA1A72D9C2970EA486026F98F965805FDE8EF9E0F1D3
      237F69ACCAF27866EE49A3053CD8A6B72B82467696915B69201E4027032074AE
      BE8A00E027D19EE2F6DA68BC1CD6B71F6E8267BA65B50542CCAEEC4AC85B380D
      D324E6B6FC4B05FF00F6DE9F7B69A74F7B1C56D71148B0BC6A54BB4257EFB2F1
      F2374AE928A00C5F08DBDCDAE88B1DE5BB5BCAD7171298D9958A879DDD725491
      9C30E84D15B545007FFFD9}
    Visible = False
    OnClick = AlarmbuttonClick
  end
  object Image2: TImage
    Left = 184
    Top = 0
    Width = 17
    Height = 25
    Cursor = crHandPoint
    OnClick = Image2Click
  end
  object Image3: TImage
    Left = 32
    Top = 32
    Width = 17
    Height = 17
    Picture.Data = {
      0A544A504547496D616765F2020000FFD8FFE000104A46494600010102001C00
      1C0000FFDB0043000503040404030504040405050506070C08070707070F0B0B
      090C110F1212110F111113161C1713141A1511111821181A1D1D1F1F1F131722
      24221E241C1E1F1EFFDB0043010505050706070E08080E1E1411141E1E1E1E1E
      1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E
      1E1E1E1E1E1E1E1E1E1E1E1E1EFFC00011080012001403012200021101031101
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
      E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00FABF
      C53AECFA36CF26C63BAFF469AE1F7CE63C2C7B320615B24EFF006E94C8BC42D2
      F888E9B15AC125B0BA3686E16E32DE6084CA7E4DB8E3054FCD9C8AC9F8993C10
      6DF3A68E2F334BBE44DEC06E63E4E1467A93E95069821B0F1459684B7A673657
      DB2049593CD117D8988C850320162338A4077945145300A28A2800A28A2803FF
      D9}
    Visible = False
  end
  object Image4: TImage
    Left = 80
    Top = 32
    Width = 25
    Height = 17
    Picture.Data = {
      0A544A504547496D61676549030000FFD8FFE000104A46494600010102001C00
      1C0000FFDB0043000503040404030504040405050506070C08070707070F0B0B
      090C110F1212110F111113161C1713141A1511111821181A1D1D1F1F1F131722
      24221E241C1E1F1EFFDB0043010505050706070E08080E1E1411141E1E1E1E1E
      1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E1E
      1E1E1E1E1E1E1E1E1E1E1E1E1EFFC00011080014001903012200021101031101
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
      E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00FAE7
      57D5D34EB9B7B716575752CE8EEAB06CE150A824EF65FEF8AABFF090C9FF0040
      1D53FEFAB7FF00E3B595F11BBFFD822FFF00F68D6278A6CACE13AADAC3696F1D
      BA5D4CC9124602291A6960401C03BB9FAF3480EFB45D4E1D52DE49628A781E19
      4C32C5326D746183838241041041048208ABD587E13FF5BAC7FD7F0FFD2786B7
      298156FF004DD3B50D9F6FB0B5BBF2F3B3CF855F6E719C6471D07E54D9B49D2A
      6BA7BA9B4CB292E1D4ABCAF029760576904919236F1F4E28A28025B1B2B3B084
      C363696F6B196DC5218C2293D3381DF81F954F451401FFD9}
    Visible = False
  end
  object Digitalformat: TEdit
    Left = 8
    Top = 24
    Width = 81
    Height = 65
    AutoSelect = False
    AutoSize = False
    BiDiMode = bdLeftToRight
    Color = clInfoBk
    Font.Charset = ANSI_CHARSET
    Font.Color = clSilver
    Font.Height = -32
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentBiDiMode = False
    ParentFont = False
    TabOrder = 0
  end
  object Hourbox: TEdit
    Left = 8
    Top = 32
    Width = 25
    Height = 24
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Transformers'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    Visible = False
  end
  object Minutebox: TEdit
    Left = 56
    Top = 32
    Width = 25
    Height = 24
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Transformers'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    Visible = False
  end
  object Secondbox: TEdit
    Left = 8
    Top = 280
    Width = 25
    Height = 24
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Transformers'
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
    Visible = False
  end
  object Timer1: TTimer
    Interval = 200
    OnTimer = Timer1Timer
    Left = 144
    Top = 272
  end
  object PopupMenu1: TPopupMenu
    Left = 112
    Top = 272
    object DigitalAnalog1: TMenuItem
      Caption = 'Digital / Analog'
      OnClick = DisplaybuttonClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object SetAlarm1: TMenuItem
      Caption = 'Set Alarm'
      OnClick = SetAlarm1Click
    end
  end
end