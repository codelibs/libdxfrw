# libdxfrw - 仕様書 / Specifications

## プロジェクト概要 / Project Overview

**プロジェクト名**: libdxfrw
**バージョン**: 0.6.3
**ライセンス**: GNU General Public License v2 (またはそれ以降のバージョン)
**作成者**: José F. Soriano (Rallaz)
**メンテナー**: codelibs, Nicu Tofan, Miguel E. Hernández Cuervo

### 目的 / Purpose

libdxfrwは、DXF (Drawing eXchange Format) ファイルの読み書きを行うための無料C++ライブラリです。ASCII形式とバイナリ形式の両方に対応しており、またDWG (AutoCAD drawing) ファイルの読み込みにも対応しています。

### 主な機能 / Main Features

1. **DXFファイルの読み書き**
   - ASCII形式のDXFファイル読み書き
   - バイナリ形式のDXFファイル読み書き
   - 複数のDXFバージョンサポート: R12, 2000, 2004, 2007, 2010

2. **DWGファイルの読み込み**
   - サポートバージョン: R14 から V2015
   - バージョン別リーダー実装:
     - dwgReader15: R14版
     - dwgReader18: R2004版
     - dwgReader21: R2007版
     - dwgReader24: R2010版
     - dwgReader27: R2013/R2015版

3. **CAD図形エンティティのサポート**
   - 基本図形: 点、線、レイ、無限線
   - 曲線図形: 円、円弧、楕円、スプライン
   - ポリライン: 2D/3Dライトウェイトポリライン
   - 3D図形: 3Dフェイス、トレース、ソリッド
   - テキスト: 単一行テキスト、複数行テキスト
   - 注釈: ハッチング、寸法、引出線
   - その他: ブロック参照、画像、ビューポート

4. **CADオブジェクトのサポート**
   - レイヤー
   - ラインタイプ
   - テキストスタイル
   - 寸法スタイル
   - ビューポート
   - アプリケーションID
   - ブロック定義

---

## アーキテクチャ / Architecture

### コアクラス構成 / Core Classes

```
dxfRW (Main API Class)
├── DRW_Interface (Abstract Interface)
├── dxfReader/dxfWriter (DXF I/O Handlers)
├── dwgReader (DWG Reader Base)
│   ├── dwgReader15 (R14)
│   ├── dwgReader18 (R2004)
│   ├── dwgReader21 (R2007)
│   ├── dwgReader24 (R2010)
│   └── dwgReader27 (R2013-2015)
├── DRW_Header (Header Variables)
├── DRW_Entity (Geometric Entities)
│   ├── DRW_Point
│   ├── DRW_Line
│   ├── DRW_Arc
│   ├── DRW_Circle
│   ├── DRW_Ellipse
│   ├── DRW_LWPolyline
│   ├── DRW_Polyline
│   ├── DRW_Spline
│   ├── DRW_Text
│   ├── DRW_MText
│   ├── DRW_Hatch
│   ├── DRW_Dimension (各種寸法タイプ)
│   └── その他のエンティティ
└── DRW_Object (Non-geometric Objects)
    ├── DRW_Layer
    ├── DRW_LType
    ├── DRW_Textstyle
    ├── DRW_Dimstyle
    └── その他のオブジェクト
```

### ディレクトリ構造 / Directory Structure

```
libdxfrw/
├── src/                      # ライブラリソースコード
│   ├── libdxfrw.h/cpp       # メインAPIクラス
│   ├── drw_interface.h      # 抽象インターフェース
│   ├── drw_entities.h/cpp   # エンティティ定義
│   ├── drw_objects.h/cpp    # オブジェクト定義
│   ├── drw_header.h/cpp     # ヘッダー変数
│   ├── drw_base.h           # 基本データ構造
│   └── intern/              # 内部実装
│       ├── dxfreader.h/cpp     # DXFリーダー
│       ├── dxfwriter.h/cpp     # DXFライター
│       ├── dwgreader*.h/cpp    # DWGリーダー群
│       ├── dwgbuffer.h/cpp     # DWGバッファ
│       ├── drw_textcodec.h/cpp # 文字コード変換
│       └── その他ユーティリティ
├── dwg2dxf/                  # DWG→DXF変換ツール
├── dwg2text/                 # DWG→テキスト抽出ツール
├── bin/                      # 実行スクリプト
└── vs2013/                   # Visual Studio 2013プロジェクト
```

---

## API仕様 / API Specifications

### メインクラス: dxfRW

#### コンストラクタ / Constructor

```cpp
dxfRW(const char* name);
```

- **パラメータ**: `name` - 処理するファイル名
- **説明**: DXF/DWGファイルを処理するためのインスタンスを作成

#### 主要メソッド / Main Methods

##### 読み込み / Reading

```cpp
bool read(DRW_Interface *interface_, bool ext);
```

- **パラメータ**:
  - `interface_`: コールバックインターフェースの実装
  - `ext`: 押し出し処理を適用して2Dに変換するか
- **戻り値**: 成功時true、失敗時false
- **説明**: ファイルを読み込み、エンティティごとにインターフェースメソッドを呼び出す

##### 書き込み / Writing

```cpp
bool write(DRW_Interface *interface_, DRW::Version ver, bool bin);
```

- **パラメータ**:
  - `interface_`: データ提供用インターフェース実装
  - `ver`: DXFバージョン (R12, 2000, 2004, 2007, 2010)
  - `bin`: バイナリ形式で出力するか
- **戻り値**: 成功時true、失敗時false

##### エンティティ書き込みメソッド / Entity Writing Methods

```cpp
bool writePoint(DRW_Point *ent);
bool writeLine(DRW_Line *ent);
bool writeCircle(DRW_Circle *ent);
bool writeArc(DRW_Arc *ent);
bool writeEllipse(DRW_Ellipse *ent);
bool writeLWPolyline(DRW_LWPolyline *ent);
bool writeSpline(DRW_Spline *ent);
bool writeText(DRW_Text *ent);
bool writeMText(DRW_MText *ent);
bool writeHatch(DRW_Hatch *ent);
// ... その他多数
```

##### テーブルオブジェクト書き込みメソッド / Table Object Writing Methods

```cpp
bool writeLayer(DRW_Layer *ent);
bool writeLineType(DRW_LType *ent);
bool writeTextstyle(DRW_Textstyle *ent);
bool writeDimstyle(DRW_Dimstyle *ent);
bool writeVport(DRW_Vport *ent);
bool writeAppId(DRW_AppId *ent);
```

### インターフェース: DRW_Interface

アプリケーションは、このインターフェースを継承してコールバックメソッドを実装する必要があります。

#### 必須実装メソッド / Required Methods

##### ヘッダー / Header

```cpp
virtual void addHeader(const DRW_Header* data) = 0;
```

##### テーブルオブジェクト / Table Objects

```cpp
virtual void addLayer(const DRW_Layer& data) = 0;
virtual void addLType(const DRW_LType& data) = 0;
virtual void addDimStyle(const DRW_Dimstyle& data) = 0;
virtual void addTextStyle(const DRW_Textstyle& data) = 0;
virtual void addVport(const DRW_Vport& data) = 0;
virtual void addAppId(const DRW_AppId& data) = 0;
```

##### ブロック / Blocks

```cpp
virtual void addBlock(const DRW_Block& data) = 0;
virtual void setBlock(const int handle) = 0;
virtual void endBlock() = 0;
```

##### エンティティ / Entities

```cpp
virtual void addPoint(const DRW_Point& data) = 0;
virtual void addLine(const DRW_Line& data) = 0;
virtual void addRay(const DRW_Ray& data) = 0;
virtual void addXline(const DRW_Xline& data) = 0;
virtual void addCircle(const DRW_Circle& data) = 0;
virtual void addArc(const DRW_Arc& data) = 0;
virtual void addEllipse(const DRW_Ellipse& data) = 0;
virtual void addLWPolyline(const DRW_LWPolyline& data) = 0;
virtual void addPolyline(const DRW_Polyline& data) = 0;
virtual void addSpline(const DRW_Spline* data) = 0;
virtual void addInsert(const DRW_Insert& data) = 0;
virtual void addTrace(const DRW_Trace& data) = 0;
virtual void add3dFace(const DRW_3Dface& data) = 0;
virtual void addSolid(const DRW_Solid& data) = 0;
virtual void addMText(const DRW_MText& data) = 0;
virtual void addText(const DRW_Text& data) = 0;
virtual void addHatch(const DRW_Hatch *data) = 0;
virtual void addViewport(const DRW_Viewport& data) = 0;
virtual void addImage(const DRW_Image *data) = 0;
```

##### 寸法 / Dimensions

```cpp
virtual void addDimAlign(const DRW_DimAligned *data) = 0;
virtual void addDimLinear(const DRW_DimLinear *data) = 0;
virtual void addDimRadial(const DRW_DimRadial *data) = 0;
virtual void addDimDiametric(const DRW_DimDiametric *data) = 0;
virtual void addDimAngular(const DRW_DimAngular *data) = 0;
virtual void addDimAngular3P(const DRW_DimAngular3p *data) = 0;
virtual void addDimOrdinate(const DRW_DimOrdinate *data) = 0;
```

##### その他 / Others

```cpp
virtual void addLeader(const DRW_Leader *data) = 0;
virtual void linkImage(const DRW_ImageDef *data) = 0;
virtual void addComment(const char* comment) = 0;
```

##### 書き込み用メソッド / Writing Methods

```cpp
virtual void writeHeader(DRW_Header& data) = 0;
virtual void writeBlocks() = 0;
virtual void writeBlockRecords() = 0;
virtual void writeEntities() = 0;
virtual void writeLTypes() = 0;
virtual void writeLayers() = 0;
virtual void writeTextstyles() = 0;
virtual void writeVports() = 0;
virtual void writeDimstyles() = 0;
virtual void writeAppId() = 0;
```

---

## ビルドシステム / Build Systems

### 1. Autotools (推奨) / Autotools (Recommended)

```bash
autoreconf -vfi
./configure
make
sudo make install
```

#### 設定ファイル / Configuration Files
- `configure.ac`: Autoconf設定
- `Makefile.am`: Automake テンプレート
- ライブラリバージョン: 0.6.3 (CURRENT=6, REVISION=3, AGE=0)

#### 依存関係 / Dependencies
- **必須**: libiconv (文字コード変換)
- **標準ライブラリ**: stdlib.h, string.h
- **数学関数**: sqrt()

### 2. CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
sudo cmake --build . --config Release --target install
```

#### CMakeLists.txt の主要設定 / Key CMake Settings
- 最小CMakeバージョン: 3.10
- ビルドターゲット: 静的ライブラリ `dxfrw`
- インストール先:
  - ヘッダー: `include/`
  - ライブラリ: `lib/` (Linux/macOS), `Debug/lib/` または `Release/lib/` (Windows)

### 3. Visual Studio 2013

- ソリューションファイル: `vs2013/libdxfrw.sln`
- プロジェクトファイル: `vs2013/libdxfrw.vcxproj`
- NuGetパッケージ: libiconv 1.14.0.11

### 4. Docker

```bash
# イメージビルド
docker build --rm -t codelibs/libdxfrw .

# ライブラリビルド
docker run -t --rm -v `pwd`:/work codelibs/libdxfrw:latest /work/build.sh
```

- ベースイメージ: CentOS 7
- 出力: `dxfrw.tar.gz` (インストール先: `/opt`)

---

## ユーティリティツール / Utility Tools

### 1. dwg2dxf

DWGファイルをDXF形式に変換するコマンドラインツール。

```bash
dwg2dxf [options] input.dwg [output.dxf]
```

**ファイル**:
- `dwg2dxf/main.cpp`: エントリーポイント
- `dwg2dxf/dx_iface.h/cpp`: インターフェース実装
- `dwg2dxf/dwg2dxf.1`: manページ

### 2. dwg2text

DWGファイルからテキスト情報を抽出するツール。

```bash
dwg2text input.dwg
```

**ファイル**:
- `dwg2text/main.cpp`: エントリーポイント
- `dwg2text/dx_iface.h/cpp`: インターフェース実装
- `dwg2text/dwg2text.1`: manページ

### 3. dxf2txt.py

DXFファイルをテキスト形式でダンプするPythonスクリプト。

```bash
python bin/dxf2txt.py input.dxf
```

---

## データ構造 / Data Structures

### 基本型 / Basic Types

```cpp
namespace DRW {
    enum Version {
        UNKNOWNV,    // 不明
        R12,         // R12 DXF
        R14,         // R14 DXF
        R2000,       // R2000 DXF
        R2004,       // R2004 DXF
        R2007,       // R2007 DXF
        R2010,       // R2010 DXF
        R2013,       // R2013 DXF
        R2015        // R2015 DXF
    };
}
```

### エンティティ基底クラス / Entity Base Class

```cpp
class DRW_Entity {
public:
    enum DRW_EntityType {
        POINT, LINE, CIRCLE, ARC, ELLIPSE,
        LWPOLYLINE, POLYLINE, SPLINE,
        INSERT, TEXT, MTEXT, HATCH,
        DIMENSION, LEADER, VIEWPORT, IMAGE,
        // ... その他
    };

    DRW_Coord basePoint;      // 基点
    std::string layer;        // レイヤー名
    std::string lineType;     // ラインタイプ
    int color;                // 色番号
    double thickness;         // 太さ
    double ltypeScale;        // ラインタイプスケール
    int handle;               // ハンドル
    // ... その他のプロパティ
};
```

### 座標型 / Coordinate Types

```cpp
class DRW_Coord {
public:
    double x;
    double y;
    double z;
};
```

---

## 文字コードサポート / Character Encoding Support

libdxfrwは、複数の文字コードページをサポートしています:

- **CP932**: 日本語 (Shift-JIS)
- **CP936**: 簡体字中国語 (GB2312)
- **CP949**: 韓国語
- **CP950**: 繁体字中国語 (Big5)

### 実装ファイル / Implementation Files

- `src/intern/drw_textcodec.h/cpp`: 文字コード変換エンジン
- `src/intern/drw_cptable932.h`: 日本語コードページ
- `src/intern/drw_cptable936.h`: 簡体字中国語
- `src/intern/drw_cptable949.h`: 韓国語
- `src/intern/drw_cptable950.h`: 繁体字中国語
- `src/intern/drw_cptables.h`: コードページテーブル統合

---

## エラーハンドリングとデバッグ / Error Handling and Debugging

### デバッグレベル設定 / Debug Level

```cpp
void setDebug(DRW::DBG_LEVEL lvl);
```

**デバッグレベル**:
- `DRW::NONE`: デバッグ出力なし
- `DRW::DEBUG`: デバッグ情報出力

### デバッグユーティリティ / Debug Utilities

- `src/intern/drw_dbg.h/cpp`: デバッグ出力ユーティリティ

---

## パフォーマンス考慮事項 / Performance Considerations

### バッファリング

- `dwgBuffer`: DWGファイル読み込み時の効率的なバイナリデータ処理
- ビット単位の読み取り操作をサポート

### メモリ管理

- エンティティとオブジェクトはポインタまたは参照で渡される
- インターフェース実装側でメモリ管理を行う

### 楕円のポリライン変換

```cpp
void setEllipseParts(int parts);
```

楕円をポリラインに変換する際の分割数を設定可能。デフォルト値よりも細かく分割することで精度向上。

---

## テストデータ / Test Data

外部テストデータリポジトリ:
- [fess-testdata/autocad](https://github.com/codelibs/fess-testdata/tree/master/autocad)

様々なDXF/DWGファイルバージョンのサンプルファイルが含まれており、統合テストに利用可能。

---

## 既知の制限事項 / Known Limitations

1. **DWGファイル書き込み非対応**: DWGファイルの読み込みのみサポート
2. **一部エンティティのサポート制限**: 最新AutoCADバージョンの全エンティティに対応していない場合がある
3. **Reed-Solomonデコーディング**: DWGファイルのエラー訂正機能は限定的

---

## バージョン履歴 / Version History

詳細は`ChangeLog`ファイルを参照してください。

主な開発マイルストーン:
- **2011**: プロジェクト開始
- **2013**: DWG R2004サポート追加
- **2014**: DWG R2010/R2013サポート追加
- **2015**: DWG R2015サポート追加
- **2020年代**: コミュニティメンテナンス継続

---

## ライセンス / License

GNU General Public License v2.0 (またはそれ以降のバージョン)

詳細は`COPYING`ファイルを参照してください。

---

## 貢献者 / Contributors

- **José F. Soriano (Rallaz)**: オリジナル作成者
- **Nicu Tofan**: コントリビューター
- **Miguel E. Hernández Cuervo**: コントリビューター
- **codelibs**: メンテナー

---

## 参考資料 / References

- 公式ウェブサイト: http://sourceforge.net/projects/libdxfrw
- GitHub: https://github.com/codelibs/libdxfrw
- DXFリファレンス: AutoCAD DXF仕様 (Autodesk公式ドキュメント)

---

## サポートとコミュニティ / Support and Community

問題報告やプルリクエストは、GitHubリポジトリを通じて行ってください:
https://github.com/codelibs/libdxfrw/issues

---

最終更新日 / Last Updated: 2025-11-08
