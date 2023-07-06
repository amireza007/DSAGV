object BenchOptionForm: TBenchOptionForm
  Left = 252
  Top = 124
  Width = 870
  Height = 600
  Caption = 'Create Benchmarks and Verification Facilities '
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 120
  TextHeight = 16
  object GroupBox1: TGroupBox
    Left = 64
    Top = 56
    Width = 345
    Height = 193
    Caption = 'Solver ( Static Aspect )'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object Label1: TLabel
      Left = 32
      Top = 56
      Width = 141
      Height = 16
      Caption = 'Input  (Graph Model)'
    end
    object Label2: TLabel
      Left = 32
      Top = 128
      Width = 115
      Height = 16
      Caption = 'Output (Solution)'
    end
    object Edit1: TEdit
      Left = 184
      Top = 56
      Width = 121
      Height = 24
      TabOrder = 0
      Text = 'Solver.in'
    end
    object Edit2: TEdit
      Left = 184
      Top = 128
      Width = 121
      Height = 24
      TabOrder = 1
      Text = 'Solver.out'
    end
  end
  object GroupBox2: TGroupBox
    Left = 440
    Top = 56
    Width = 329
    Height = 193
    Hint = 
      'This option can be chosen, if the time simulator  is exattly equ' +
      'al to one Second'
    Caption = 'Schedule ( Dynamic Aspect )'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    object Label3: TLabel
      Left = 40
      Top = 72
      Width = 175
      Height = 32
      Alignment = taCenter
      Caption = 'File To Record Remaing Jobs and Tour Solution'
      WordWrap = True
    end
    object Edit3: TEdit
      Left = 56
      Top = 112
      Width = 169
      Height = 24
      TabOrder = 0
      Text = 'Problem_Solution.txt'
    end
    object CheckBox1: TCheckBox
      Left = 24
      Top = 40
      Width = 153
      Height = 17
      Hint = 
        'This option can be chosen, if the time simulator  is exattly equ' +
        'al to one Second'
      Caption = 'Dynamic Recording'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object Button2: TButton
      Left = 48
      Top = 152
      Width = 75
      Height = 25
      Caption = 'Reset'
      TabOrder = 2
      OnClick = Button2Click
    end
  end
  object GroupBox3: TGroupBox
    Left = 64
    Top = 272
    Width = 345
    Height = 185
    Caption = 'Time  Simulator (One Second)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object RadioButton1: TRadioButton
      Left = 32
      Top = 40
      Width = 233
      Height = 17
      Caption = 'Slow  ( Exactly One Second )'
      TabOrder = 0
      OnClick = RadioButton1Click
    end
    object RadioButton2: TRadioButton
      Left = 32
      Top = 88
      Width = 233
      Height = 17
      Caption = 'Fast  ( Equal to 0.1   Second)'
      TabOrder = 1
      OnClick = RadioButton2Click
    end
    object RadioButton3: TRadioButton
      Left = 32
      Top = 136
      Width = 257
      Height = 17
      Caption = 'Very Fast (Equal to  0.01 Second)'
      Checked = True
      TabOrder = 2
      TabStop = True
      OnClick = RadioButton3Click
    end
  end
  object Button1: TButton
    Left = 352
    Top = 512
    Width = 105
    Height = 33
    Caption = 'Close'
    TabOrder = 3
    OnClick = Button1Click
  end
  object GroupBox4: TGroupBox
    Left = 440
    Top = 264
    Width = 329
    Height = 193
    Caption = 'Route'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    object Label5: TLabel
      Left = 24
      Top = 48
      Width = 91
      Height = 16
      Caption = 'Network  File'
    end
    object Label6: TLabel
      Left = 24
      Top = 112
      Width = 92
      Height = 16
      Caption = 'Distance File'
    end
    object Edit5: TEdit
      Left = 144
      Top = 48
      Width = 121
      Height = 24
      TabOrder = 0
      Text = 'Network.txt'
    end
    object Edit6: TEdit
      Left = 144
      Top = 112
      Width = 121
      Height = 24
      TabOrder = 1
      Text = 'Distance.txt'
    end
  end
end
