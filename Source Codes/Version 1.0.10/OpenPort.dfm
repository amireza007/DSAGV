object OpenPortForm: TOpenPortForm
  Left = -4
  Top = -4
  Width = 1288
  Height = 935
  BorderIcons = []
  Caption = 'Open Port'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  WindowState = wsMaximized
  PixelsPerInch = 120
  TextHeight = 16
  object DBGrid1: TDBGrid
    Left = 105
    Top = 108
    Width = 1009
    Height = 553
    DataSource = DataSource1
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -10
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    Columns = <
      item
        Alignment = taCenter
        Expanded = False
        FieldName = 'Portname'
        Title.Alignment = taCenter
        Title.Caption = 'Port Name'
        Width = 127
        Visible = True
      end
      item
        Alignment = taCenter
        Expanded = False
        FieldName = 'NumberOfBlockYard'
        Title.Alignment = taCenter
        Title.Caption = 'Number Of Blocks in the Yard'
        Width = 187
        Visible = True
      end
      item
        Alignment = taCenter
        Expanded = False
        FieldName = 'NumberOfWorkingPosition'
        Title.Alignment = taCenter
        Title.Caption = 'Number Of Quay Cranes'
        Width = 154
        Visible = True
      end
      item
        Alignment = taCenter
        Expanded = False
        FieldName = 'NumberOfAGVs'
        Title.Alignment = taCenter
        Title.Caption = 'Number Of AGVs'
        Width = 113
        Visible = True
      end
      item
        Alignment = taCenter
        Expanded = False
        FieldName = 'NumberOfJunctions'
        Title.Alignment = taCenter
        Title.Caption = 'Number Of Junctions'
        Width = 138
        Visible = True
      end
      item
        Alignment = taCenter
        Expanded = False
        FieldName = 'NumberOfLanes'
        Title.Alignment = taCenter
        Title.Caption = 'Number Of Lanes'
        Width = 123
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Description'
        Width = 114
        Visible = True
      end>
  end
  object DBNavigator1: TDBNavigator
    Left = 369
    Top = 71
    Width = 240
    Height = 25
    DataSource = DataSource1
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
  end
  object Button1: TButton
    Left = 556
    Top = 721
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 2
    OnClick = Button1Click
  end
  object DataSource1: TDataSource
    DataSet = Table2
    Left = 504
    Top = 24
  end
  object Table2: TTable
    BeforeDelete = Table2BeforeDelete
    DatabaseName = 'PortAppDatabase'
    Filtered = True
    FieldDefs = <
      item
        Name = 'Portname'
        DataType = ftString
        Size = 30
      end
      item
        Name = 'NumberOfBlockYard'
        DataType = ftFloat
      end
      item
        Name = 'NumberOfWorkingPosition'
        DataType = ftFloat
      end
      item
        Name = 'NumberOfZones'
        DataType = ftFloat
      end
      item
        Name = 'Description'
        DataType = ftString
        Size = 120
      end
      item
        Name = 'NumberOfAGVs'
        DataType = ftFloat
      end
      item
        Name = 'MaxDistance'
        DataType = ftFloat
      end
      item
        Name = 'W1'
        DataType = ftFloat
      end
      item
        Name = 'W2'
        DataType = ftFloat
      end
      item
        Name = 'NumberOfJunctions'
        DataType = ftFloat
      end
      item
        Name = 'NumberOfLanes'
        DataType = ftFloat
      end>
    IndexDefs = <
      item
        Name = 'Table2Index1'
        Fields = 'Portname'
        Options = [ixPrimary, ixUnique]
      end>
    IndexFieldNames = 'Portname'
    StoreDefs = True
    TableName = 'PortTable.db'
    Left = 568
    Top = 24
    object Table2Portname: TStringField
      FieldName = 'Portname'
      Size = 30
    end
    object Table2NumberOfBlockYard: TFloatField
      FieldName = 'NumberOfBlockYard'
    end
    object Table2NumberOfWorkingPosition: TFloatField
      FieldName = 'NumberOfWorkingPosition'
    end
    object Table2Description: TStringField
      FieldName = 'Description'
      Size = 120
    end
    object Table2NumberOfAGVs: TFloatField
      FieldName = 'NumberOfAGVs'
    end
    object Table2MaxDistance: TFloatField
      FieldName = 'MaxDistance'
    end
    object Table2W1: TFloatField
      FieldName = 'W1'
    end
    object Table2W2: TFloatField
      FieldName = 'W2'
    end
    object Table2NumberOfZones: TFloatField
      FieldName = 'NumberOfZones'
    end
    object Table2NumberOfLanes: TFloatField
      FieldName = 'NumberOfLanes'
    end
    object Table2NumberOfJunctions: TFloatField
      FieldName = 'NumberOfJunctions'
    end
  end
  object PortDatabase: TDatabase
    AliasName = 'PortAppDatabase'
    DatabaseName = 'DataBase1'
    SessionName = 'Default'
    Left = 616
    Top = 24
  end
end
