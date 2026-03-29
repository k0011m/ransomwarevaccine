# Ransomware Vaccine

軽量でユーザーモード動作のランサムウェア特化型アンチウイルスソフト

- **Windows 10+ 対応**
- **管理者権限不要**（実行するランサムウェアが必要の場合、ワクチンも使用します）
- **軽量**
- **IAT フッキング**による低レベルファイルAPI保護

---

## 概要

**Ransomware Vaccine** は、ランサムウェアのファイルアクセス操作を検出・ブロック・監視するセキュリティツールです。

ユーザーモードで動作し、**管理者権限を必要としない**ため、一般ユーザーでも利用可能。複数の保護モード（AB / WDB / AN）を備えており、防御レベルと利便性のバランスを自由に選択できます。

### 主な特徴

- **IAT フッキング**: Import Address Table を直接修正してファイルAPI呼び出しを横取り
- **DLL インジェクション**: ターゲットプロセスに自動的にワクチンDLLをロード
- **子プロセス連鎖伝播**: ランサムウェアが生成した子プロセスにも自動伝播
- **3つの保護モード**: 完全ブロック、書き込み・削除のみブロック、監視のみ
- **柔軟な通知オプション**: GUI（MessageBox）、コンソール、サイレント

---

## クイックスタート

### 基本的な使用方法

```bash
ransomwarevaccine_injection.exe <target.exe> <mode> <notification> <logging> [DLL_path]
```

### 最もシンプルな例

```bash
# ランサムウェアを AB モード（完全ブロック）で監視
ransomwarevaccine_injection.exe suspicious.exe -AB -TM -T
```

### より詳しい使用例

```bash
# モード: -AB / -WDB / -AN
# 通知: -TM (MessageBox) / -TC (Console) / -F (Silent)
# ログ: -T (Enable) / -F (Disable)
# DLL_path: (オプション) カスタム DLL パス
```

---

## 保護モード詳細

Ransomware Vaccine は3つの保護モードを搭載しています。各モードで以下のファイル操作API への対応が異なります。

### モード比較表

| 関数 | AB (All Block) | WDB (Write/Delete Block) | AN (All No block) |
|------|---|---|---|
| **CreateFileW/A** | ブロック | 通過 | 通過 |
| **WriteFile** | ブロック | ブロック | 通過 |
| **ReadFile** | ブロック | 通過 | 通過 |
| **DeleteFileW/A** | ブロック | ブロック | 通過 |
| **CopyFileA/W** | ブロック | ブロック | 通過 |
| **MoveFileA/W** | ブロック | ブロック | 通過 |
| **CreateDirectoryW/A** | 通過 | 通過 | 通過 |
| **VirtualAlloc/Ex** | ブロック | 通過 | 通過 |
| **GetProcAddress** | リダイレクト | 通過 | 通過 |

### 各モードの説明

#### 🔴 **AB (All Block) モード** (`-AB`)

**最も強力な保護モード。ランサムウェアのほぼ全ての操作をブロック。**

- CreateFile、WriteFile、ReadFile、DeleteFile をすべてブロック
- ファイルコピー・移動も禁止
- GetProcAddress での IAT 迂回をリダイレクト
- VirtualAlloc（メモリ確保）もブロック

---

#### 🟡 **WDB (Write/Delete Block) モード** (`-WDB`)

**バランス型の保護モード。ファイル読み取りは許可しつつ、書き込み・削除をブロック。**

- CreateFile は許可（ファイルオープン可能）
- WriteFile をブロック（内容変更不可）
- DeleteFile をブロック（削除不可）
- ReadFile は許可（読み取り可能）
- CopyFile・MoveFile は禁止

---

#### 🟢 **AN (All No block) モード** (`-AN`)

**保護なし。すべてのファイル操作を許可し、監視・通知のみ行う。**

- 全ファイル操作API を通過させる
- 通知機能を有効にすることで、挙動を記録・分析できる

---

## コマンドラインオプション

### モード選択（第2引数）必須

| オプション | 説明 | ブロック範囲 |
|-----------|------|-----------|
| `-AB` | All Block モード | 全ファイル操作 |
| `-WDB` | Write/Delete Block モード | 書き込み・削除 |
| `-AN` | All No block モード | ブロックなし |

**使用例:**
```bash
ransomwarevaccine_injection.exe target.exe -AB -TM -T
```

---

### 通知オプション（第3引数）必須

| オプション | 説明 | 出力先 |
|-----------|------|--------|
| `-TM` | MessageBox で通知 | GUI ポップアップ |
| `-TC` | コンソール出力で通知 | CUI（標準出力） |
| `-F` | 通知なし（サイレント） | なし |

**使用例:**
```bash
# GUI 通知の場合
ransomwarevaccine_injection.exe target.exe -AB -TM -T

# コンソール通知の場合
ransomwarevaccine_injection.exe target.exe -WDB -TC -F

# サイレント実行
ransomwarevaccine_injection.exe target.exe -AN -F -T
```

---

### ログオプション（第4引数）必須

| オプション | 説明 |
|-----------|------|
| `-T` | ログ出力有効 |
| `-F` | ログ出力無効 |

**注記**: 現在のバージョンではログ出力機能が準備中です。オプション自体は認識されます。

**使用例:**
```bash
# ログ有効
ransomwarevaccine_injection.exe target.exe -AB -TM -T

# ログ無効
ransomwarevaccine_injection.exe target.exe -WDB -TC -F
```

---

### ランサムウェアワクチン DLL の配置（第5引数）オプション

デフォルト動作では、**`ransomwarevaccine_injection.exe` と同じフォルダ**から `ransomwarevaccine_vaccinedll.dll` を探します。

カスタム配置する場合は、第5引数にフォルダパスを指定：

```bash
# DLL がカスタムパスにある場合
ransomwarevaccine_injection.exe target.exe -AB -TM -T C:\\custom\\path\\to\\dll
```

この場合、`C:\custom\path\to\dll\ransomwarevaccine_vaccinedll.dll` を探します。

---

## 技術詳細

### IAT (Import Address Table) フッキング

Ransomware Vaccine は、ターゲットプロセスの IAT を直接修正して、ファイル操作 API への呼び出しを横取りします。

**処理フロー:**

1. `ImageDirectoryEntryToData` でターゲットの Import Descriptor テーブルを走査
2. CreateFile、WriteFile 等のエントリを発見
3. `VirtualProtect(PAGE_EXECUTE_READWRITE)` でメモリ保護を一時解除
4. IAT エントリを、自作のフック関数アドレスに上書き
5. 元のメモリ保護を復元

**利点**: カーネルモード不要で、ユーザーモードだけで実装可能

---

### DLL インジェクション + 子プロセス伝播

**Injector (ransomwarevaccine_injection.exe) の動作:**

```
1. CreateProcessW(target.exe, CREATE_SUSPENDED)
   → ターゲットを一時停止状態で起動

2. VirtualAllocEx + WriteProcessMemory
   → ターゲットのメモリ空間に DLL パスを書き込み

3. CreateRemoteThread(LoadLibraryA, DLLパス)
   → ターゲット内で LoadLibraryA を実行させ DLL をロード

4. ResumeThread
   → ターゲットを実行開始

5. ターゲットが CreateProcessW で子プロセスを生成する際:
   → フック関数が CREATE_SUSPENDED を強制付加
   → 子プロセスにも DLL をインジェクション
   → 全階層のプロセスに伝播
```

このメカニズムにより、ランサムウェアが何段階のプロセスを生成しても、すべてが保護される仕組みです。


---

## 使用例

### 例1: AB モード（全ブロック）での実行例

```bash
ransomwarevaccine_injection.exe ransomware.exe -AB -TM -T
```

**実行結果:**

1. `ransomware.exe` が起動される
2. ファイルオープン試行時に MessageBox で通知
3. CreateFile が `INVALID_HANDLE_VALUE` を返す → ファイルオープン失敗
4. WriteFile が `FALSE` を返す → ファイル書き込み失敗
5. DeleteFile が `FALSE` を返す → ファイル削除失敗
6. **ランサムウェアのファイル暗号化が失敗し、ファイルが保護される**

---

### 例2: WDB モード（書き込み・削除ブロック）での実行例

```bash
ransomwarevaccine_injection.exe target.exe -WDB -TC -F
```

**実行結果:**

1. `target.exe` が起動される
2. ファイル読み取り (ReadFile) は許可される → ファイル情報取得可能
3. ファイル書き込み (WriteFile) がブロックされる → 内容変更不可
4. ファイル削除 (DeleteFile) がブロックされる → ファイル削除不可
5. コンソールに操作を通知（GUI ポップアップなし）
6. ログ無効（サイレント実行）
7. **ランサムウェアは読み取り情報を得られるが、ファイル破壊はできない**

---

### 例3: AN モード（監視のみ）での実行例

```bash
ransomwarevaccine_injection.exe suspicious.exe -AN -F -T
```

**実行結果:**

1. `suspicious.exe` が起動される
2. 全ファイル操作が通常通り実行される（ブロックなし）
3. 通知なし（サイレント実行）
4. ログに全操作を記録
5. **プロセスは完全に通常動作するが、ファイルアクセスが記録される**
6. 後で、ログを分析して疑わしい操作を検出

---

## プロジェクト構成

```
ransomwarevaccine/
├── ransomwarevaccine_injection.c      ← Injector (ランチャー EXE)
├── ransomwarevaccine_vaccinedll.c     ← Vaccine DLL 本体
├── ransomwarevaccine_vaccinedll.h     ← DLL ヘッダー
└── README.md                          ← このファイル
```

### ファイル説明

#### **ransomwarevaccine_injection.c**

ターゲットプロセスにワクチン DLL をインジェクションするランチャーEXE。

**主な機能:**
- コマンドライン引数の解析
- 環境変数経由でモード・通知設定をDLLに渡す
- `CreateProcessW(CREATE_SUSPENDED)` でターゲットを一時停止状態で起動
- `VirtualAllocEx` → `WriteProcessMemory` → `CreateRemoteThread` でDLLをインジェクション

---

#### **ransomwarevaccine_vaccinedll.c**

IAT フッキング + ファイルAPI横取りの実装。

**主な特徴:**
- AB / WDB / AN 3モード対応（環境変数 `ransomwarevaccine_protect_mode` に従う）
- 全16個のファイル操作API をフック
  - CreateFileW/A, WriteFile, ReadFile
  - DeleteFileW/A, CreateDirectoryW/A
  - CopyFileA/W, MoveFileA/W
  - CreateProcessW/A (子プロセス伝播)
  - GetProcAddress (IAT迂回対策)
  - VirtualAlloc/Ex (メモリ確保阻止)
- 通知機能（MessageBox / コンソール / サイレント）

---

#### **ransomwarevaccine_vaccinedll.h**

DLL エクスポート関数の宣言。

---

## セキュリティに関する注意

⚠️ **重要な免責事項**

- 本ソフトウェアは **セキュリティ研究目的** で開発されています
- **自己責任での使用** をお願いします
- 本番環境での使用は、十分なテストの上で行ってください
- ランサムウェア検体の扱いは適切に行ってください（**仮想環境での隔離実行を強く推奨**）
- 本ツールで検出・ブロックしきれるランサムウェアは限定的です。完全な保護は保証できません

---