# GrammarAlgorithms
Практикум 2, Алгоритм Эрли, Судаков Елисей, Б05-923

Алгоритм Эрли - алгоритм для проверки, распознаёт ли данная КС грамматика данное слово. Проект содержит реализацию этого алгоритма, а также реализацию приведения грамматики, заданной в нормальной форме Хомского, к нормальной форме Грейбах.

В программе используются стандартные сущности:
Rule - правило грамматики, Grammar - грамматика, Situation - ситуация в алгоритме Эрли, predict, complete, scan - стандартные операции.

Реализованные функции покрыты тестами (tests.h)

Собирается проект при помощи CMake, (см. build.sh).
После сборки в папке bin появляются два исполняемых файла: main и test.

После запуска main необходимо ввести грамматику в фиксированном формате (необходимо, чтобы стартовый символ был S', а единственное правило из него - S'-->S, примеры входных данных -
в input_examples.txt). В случае, если входные данные были корректными, программа выведет 1, если слово распознавалось грамматикой и 0 - иначе.

test запускает тесты.