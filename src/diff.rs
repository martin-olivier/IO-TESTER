use crate::test::Status;
use crate::test::TestResult;
use crate::tools;

use std::io::Write;
use colored::Colorize;

fn print_diff(got: String, expected: String) {
    for diff in diff::lines(got.as_str(), expected.as_str()) {
        match diff {
            diff::Result::Left(l)    => println!("{} {}", " + ".bold().white().on_green(), l.green()),
            diff::Result::Both(l, _) => println!("{} {}", "   ".on_white(), l),
            diff::Result::Right(r)   => println!("{} {}", " - ".bold().white().on_red(), r.red()),
        }
    }
}

pub fn shell(_name: &str, result: TestResult) {
    if result.status == Status::Passed || result.status == Status::Skipped {return}

    println!("{}", " ^ ".bold().white());
    println!("{} {}\n{}", " > ".white().on_blue(), "[code]".blue(), "   ".on_white());
    print_diff(format!("value = {}", result.got_code.unwrap_or(-1)), format!("value = {}", result.expected_code));

    if let Some(expected_stdout) = result.expected_stdout {
        println!("{}\n{} {}\n{}", "   ".on_white(), " > ".white().on_blue(), "[stdout]".blue(), "   ".on_white());
        print_diff(result.got_stdout, expected_stdout);
    }
    if let Some(expected_stderr) = result.expected_stderr {
        println!("{}\n{} {}\n{}", "   ".on_white(), " > ".white().on_blue(), "[stderr]".blue(), "   ".on_white());
        print_diff(result.got_stderr, expected_stderr);
    }
    println!();
}

pub fn vscode(name: &str, result: TestResult) {
    if result.status == Status::Passed || result.status == Status::Skipped {return}

    let got = format!("[code]\n\nvalue = {}{}{}{}{}",
        result.got_code.unwrap_or(-1),
        if result.expected_stdout.is_some() {"\n\n[stdout]\n\n"} else {""},
        if result.expected_stdout.is_some() {result.got_stdout} else {"".to_string()},
        if result.expected_stderr.is_some() {"\n\n[stderr]\n\n"} else {""},
        if result.expected_stderr.is_some() {result.got_stderr} else {"".to_string()},
    );
    let expected = format!("[code]\n\nvalue = {}{}{}{}{}",
        result.expected_code,
        if result.expected_stdout.is_some() {"\n\n[stdout]\n\n"} else {""},
        if let Some(expected_stdout) = result.expected_stdout {expected_stdout} else {"".to_string()},
        if result.expected_stderr.is_some() {"\n\n[stderr]\n\n"} else {""},
        if let Some(expected_stderr) = result.expected_stderr {expected_stderr} else {"".to_string()},
    );

    let tmp_path = match cfg!(target_os = "windows") {
        true  => std::env::var("Temp").unwrap() + "/",
        false => "/tmp/".to_string(),
    };

    let file_got = format!("{}GOT({})", tmp_path, name);
    std::fs::File::create(&file_got).unwrap().write_fmt(format_args!("{}", got)).unwrap();

    let file_expected = format!("{}EXPECTED({})", tmp_path, name);
    std::fs::File::create(&file_expected).unwrap().write_fmt(format_args!("{}", expected)).unwrap();

    let cmd = format!("\"{}\" --diff \"{}\" \"{}\"", tools::get_vscode_bin().unwrap(), file_got, file_expected);

    std::process::Command::new(if cfg!(target_os = "windows") {"cmd"} else {"sh"})
        .args([if cfg!(target_os = "windows") {"/C"} else {"-c"}, cmd.as_str()])
        .output()
        .expect("failed to execute vscode");
}